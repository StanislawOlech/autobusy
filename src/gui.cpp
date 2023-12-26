#include "gui.hpp"

#include "imgui.h"
#include "implot.h"
#include <cmath>
#include <charconv>


void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


void MakeWarningPopup(const char* name, const char* text)
{
    if (ImGui::BeginPopup(name))
    {
        ImGui::Text(text);
        if (ImGui::Button("Ok"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void MakeInputPositive(const char* name, const char* error_text, int *number)
{
    if (ImGui::InputInt(name, number))
    {
        if (*number <= 0)
        {
            *number = 1;
            ImGui::OpenPopup(name);
        }
    }
    MakeWarningPopup(name, error_text);
}


void GUI::Draw()
{
    ImGui::Begin("Okno");
    ImGui::BeginTabBar("Tabs");

    if (ImGui::BeginTabItem("Algorytm"))
    {
        DrawAlgorithm();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Styl"))
    {
        DrawStyle();
        ImGui::EndTabItem();
    }


    ImGui::EndTabBar();
    ImGui::End();

    ImGui::Begin("Wykres");
    DrawPlot();
    ImGui::End();
}

void GUI::DrawStyle()
{
    static int color_idx = 0;
    if (ImGui::Combo("Kolor##Selector", &color_idx, "Ciemny\0Jasny\0"))
    {
        switch (color_idx)
        {
            default:
            case 0: ImGui::StyleColorsDark(); break;
            case 1: ImGui::StyleColorsLight(); break;
        }
    }
}

void GUI::DrawAlgorithm()
{
    static bool open_passengers = false;

    if (ImGui::CollapsingHeader("Stacje"))
        DrawStationList();
    if (ImGui::CollapsingHeader("Połączenia stacji"))
    {
        if (ImGui::Button(u8"Lista pasażerów##nowe_okno"))
            open_passengers = true;

        DrawStationTable();
    }
    if (ImGui::CollapsingHeader(u8"Parametry sterujące"))
        DrawArguments();

    if (open_passengers)
        DrawPassengers(&open_passengers);
}

void GUI::DrawPlot()
{
    static float xs1[1001], ys1[1001];
    for (int i = 0; i < 1001; ++i) {
        xs1[i] = i * 0.001f;
        ys1[i] = 0.5f + 0.5f * sin(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
    }
    static double xs2[20], ys2[20];

    for (int i = 0; i < 20; ++i) {
        xs2[i] = i * 1/19.0f;
        ys2[i] = xs2[i] * xs2[i];
    }

    if (ImPlot::BeginPlot("Line Plots", {-1, -1})) {
        ImPlot::SetupAxes("x","y");
        ImPlot::PlotLine("f(x)", xs1, ys1, 1001);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);
        ImPlot::EndPlot();
    }
}

void GUI::DrawStationList()
{
    static int vec[2] = {0, 0};

    ImGui::InputInt2("Pozycja stacji", vec);
    if (ImGui::Button(u8"Dodaj nową stację"))
    {
        Point2D point2D = {vec[0], vec[1]};
        if (stations_.end() != std::find(stations_.begin(), stations_.end(), point2D)) [[unlikely]]
        {
            ImGui::OpenPopup("Uwaga");
        }
        else
            stations_.emplace_back(vec[0], vec[1]);
    }

    MakeWarningPopup("Uwaga", u8"Stacja już istnieje");

    /// Tabela z zapamiętanymi stacjami
    constexpr static ImGuiTableFlags table_flags = ImGuiTableFlags_ScrollY |
                                                   ImGuiTableFlags_RowBg |
                                                   ImGuiTableFlags_BordersOuter |
                                                   ImGuiTableFlags_BordersV |
                                                   ImGuiTableFlags_Resizable |
                                                   ImGuiTableFlags_Reorderable |
                                                   ImGuiTableFlags_HighlightHoveredColumn |
                                                   ImGuiTableFlags_Hideable;

    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
    if (ImGui::BeginTable("Station_table", 4, table_flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(1, 1); // Make top row always visible
        ImGui::TableSetupColumn("Nr.", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int) stations_.size());

        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::PushID(row);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", row);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", stations_[row].x);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", stations_[row].y);

                ImGui::TableSetColumnIndex(3);
                if (row != 0 && ImGui::SmallButton("Usuń"))
                {
                    stations_.erase(stations_.begin() + row);

                    // FIXME: przesuń połączenia pomiędzy stacjami gdy zostanie usunięta stacja
                    connections_ = {};

                    // Nie Działa
//                    std::vector<uint8_t> new_connections_;
//                    new_connections_.reserve(connections_.size());
//
//                    for (std::size_t y = 0; y < connections_.size(); ++y)
//                    {
//                        for (std::size_t x = 0; x < connections_.size(); ++x)
//                        {
//                            if (y == row)
//                                break;
//                            if (x == row)
//                                continue;
//                            new_connections_.push_back(connections_[y * connections_.size() + x]);
//                        }
//                    }
//                    connections_ = new_connections_;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }
}

void GUI::DrawStationTable()
{
    constexpr static ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit |
                                                   ImGuiTableFlags_ScrollX |
                                                   ImGuiTableFlags_ScrollY |
                                                   ImGuiTableFlags_BordersOuter |
                                                   ImGuiTableFlags_BordersV |
                                                   ImGuiTableFlags_BordersInnerH |
                                                   ImGuiTableFlags_Hideable |
                                                   ImGuiTableFlags_HighlightHoveredColumn;

    HelpMarker(u8"Dodawanie połączeń między stacjami\n"
               "rząd - stacja startowa\n"
               "kolumna - stacja stacja końcowa\n"
               "Jest to graf nie skierowany - automatyczne dodawanie krawędzi powrotnej"
               );

    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeight() * 14);

    if (ImGui::BeginTable("table_angled_headers", stations_.size() + 1, table_flags, outer_size))
    {
        connections_.resize(stations_.size() * stations_.size());

        ImGui::TableSetupScrollFreeze(1, 1);

        ImGui::TableSetupColumn("Stacje", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoReorder);

        for (int n = 0; n < stations_.size(); n++)
            ImGui::TableSetupColumn(std::to_string(n).c_str());

        ImGui::TableHeadersRow();
        for (int row = 0; row < stations_.size(); row++)
        {
            ImGui::PushID(row);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%d", row);
            for (int column = 1; column < stations_.size() + 1; column++)
                if (ImGui::TableSetColumnIndex(column) && column - 1 != row)
                {
                    ImGui::PushID(column);

                    std::size_t id = row * stations_.size() + column-1;

                    if (row > column - 1)
                        id = (column-1) * stations_.size() + row;

                    ImGui::Checkbox("", (bool*)&connections_[id]);
                    ImGui::PopID();
                }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUI::DrawArguments()
{
    ImGui::PushItemWidth(120);
    MakeInputPositive(u8"Liczba iteracji", u8"Liczba iteracji musi być dodatnia", &max_iter_);
    MakeInputPositive(u8"Liczba autobusów", u8"Liczba autbusów musi być dodatnia", &autobus_number_);

    if (ImGui::InputInt(u8"Liczba rozwiązań", &solutions_number_))
    {
        if (solutions_number_ <= 0)
        {
            solutions_number_ = 1;
            ImGui::OpenPopup("Liczba rozwiązań##dodatnie");
        }
        else if (solutions_number_ < best_number_)
        {
            best_number_ = solutions_number_;
            if (elite_number_ > solutions_number_)
                elite_number_ = solutions_number_;
        }
    }
    MakeWarningPopup(u8"Liczba rozwiązań##dodatnie", u8"Liczba najlepszych rozwiązań musi być dodatnia");

    if (ImGui::InputInt("Liczba najlepszych rozwiązań", &best_number_))
    {
        if (best_number_ <= 0)
        {
            best_number_ = 1;
            ImGui::OpenPopup("Liczba najlepszych rozwiązań##dodatnie");
        }
        else if (best_number_ > solutions_number_)
        {
            best_number_ = solutions_number_;
            ImGui::OpenPopup("Liczba najlepszych rozwiązań##zaduże");
        }
        if (elite_number_ > best_number_)
            elite_number_ = best_number_;

    }
    MakeWarningPopup(u8"Liczba najlepszych rozwiązań##dodatnie", u8"Liczba najlepszych rozwiązań musi być dodatnia");
    MakeWarningPopup(u8"Liczba najlepszych rozwiązań##zaduże",
                     u8"Liczba najlepszych rozwiązań nie może być większa od liczby rozwiązań");

    if (ImGui::InputInt("Liczba elitarnych rozwiązań", &elite_number_))
    {
        if (elite_number_ <= 0)
        {
            elite_number_ = 1;
            ImGui::OpenPopup("Liczba elitarnych rozwiązań##dodatnie");
        }
        else if (best_number_ < elite_number_)
        {
            elite_number_ = best_number_;
            ImGui::OpenPopup("Liczba elitarnych rozwiązań##zaduże");
        }
    }
    MakeWarningPopup(u8"Liczba elitarnych rozwiązań##dodatnie", u8"Liczba elitarnych rozwiązań musi być dodatnia");
    MakeWarningPopup(u8"Liczba elitarnych rozwiązań##zaduże",
                     u8"Liczba elitarnych rozwiązań nie może być większa od liczby najlepszych rozwiązań");

    MakeInputPositive(u8"Otoczenie najlepszych rozwiązań",
                      u8"Otoczenie najlepszych rozwiązań musi być dodatnie", &best_size_);

    if (ImGui::InputInt("Otoczenie elitarnych rozwiązań", &elite_size_))
    {
        if (elite_size_ <= 0)
        {
            elite_size_ = 1;
            ImGui::OpenPopup("Otoczenie elitarnych rozwiązań##dodatnie");
        }
        else if (elite_size_ <= best_size_)
        {
            elite_size_ = best_size_ + 1;
            ImGui::OpenPopup(u8"Otoczenie elitarnych rozwiązań##zamałe");
        }
    }
    MakeWarningPopup(u8"Otoczenie elitarnych rozwiązań##dodatnie", u8"Otoczenie elitarnych rozwiązań musi być dodatnia");
    MakeWarningPopup(u8"Otoczenie elitarnych rozwiązań##zamałe",
                     u8"Otoczenie elitarnych rozwiązań musi być większ od otoczenie najlepszych rozwiązań");

    ImGui::PopItemWidth();

}

void GUI::DrawPassengers(bool *open)
{
    if (!ImGui::Begin(u8"Lista pasażerów##funkcja", open))
    {
        ImGui::End();
        return;
    }

    static constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersOuter |
                                                  ImGuiTableFlags_Resizable |
                                                  ImGuiTableFlags_ScrollY;

    static constexpr ImGuiChildFlags childFlags = ImGuiChildFlags_None;
    static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;

    static int selected_station_id = -1;
    static int selected_time = -1;

    char buffer[16];

    float window_size_x = ImGui::GetContentRegionAvail().x;

    // Table 1
    {
        ImGui::BeginChild("Stacje##Child1", ImVec2(window_size_x * 0.15f, 0), childFlags, windowFlags);
        if (ImGui::BeginTable(u8"Stacje##Tabela", 1, tableFlags))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn(u8"Stacja");
            ImGui::TableHeadersRow();

            for (int i = 0; i < stations_.size(); ++i)
            {
                ImGui::PushID(i);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                sprintf(buffer, u8"Stacja %u", i);
                if (ImGui::Selectable(buffer, selected_station_id == i))
                {
                    selected_station_id = i;
                    selected_time = -1;
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }

    ImGui::SameLine();

    // Table 2
    {
        ImGui::BeginChild(u8"Czas##Child2", ImVec2(window_size_x * 0.15f, 0), childFlags, windowFlags);
        if (ImGui::BeginTable(u8"Czas##Tabela", 1, tableFlags))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn(u8"Czas");
            ImGui::TableHeadersRow();

            auto &time_vec = table3D[selected_station_id];

            for (int i = 0; i < time_vec.size(); ++i)
            {
                ImGui::PushID(i);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                sprintf(buffer, u8"Czas %u", i);
                if (ImGui::Selectable(buffer, selected_time == i))
                {
                    selected_time = i;
                }
                ImGui::PopID();
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (selected_station_id != -1)
            {
                if (ImGui::Button(u8"Dodaj czas"))
                    time_vec.emplace_back();
            }

            ImGui::EndTable();
        }
        ImGui::EndChild();
    }

    ImGui::SameLine();

    // Table 3
    {
        ImGui::BeginChild(u8"Pasażerowie##Child3", ImVec2(0, 0), childFlags, windowFlags);
        if (ImGui::BeginTable(u8"Pasażerowie##Tabela", 1, tableFlags))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn(u8"Pasażerowie");
            ImGui::TableHeadersRow();

            std::vector<std::vector<PassengerGui>> &time_vec = table3D[selected_station_id];

            // If time is selected
            if (selected_time != -1)
            {
                auto &passengers_vec = time_vec[selected_time];

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Button(u8"Dodaj grupę pasażerów"))
                    passengers_vec.emplace_back();


                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                // Inner table
                if (ImGui::BeginTable("Pasażerówie##Dane", 3, tableFlags))
                {
                    ImGui::TableSetupScrollFreeze(0, 3);
                    ImGui::TableSetupColumn(u8"Stacja końcowa");
                    ImGui::TableSetupColumn(u8"Liczba");
                    ImGui::TableSetupColumn(u8"");
                    ImGui::TableHeadersRow();


                    for (int i = 0; i < passengers_vec.size(); ++i)
                    {
                        ImGui::PushID(i);
                        ImGui::TableNextRow();

                        // Column 1
                        ImGui::TableSetColumnIndex(0);

                        int &item_current_idx = passengers_vec[i].dest_id;

                        if (item_current_idx != -1) [[likely]]
                            sprintf(buffer, u8"Stacja %u", item_current_idx);
                        else
                            sprintf(buffer, "");

                        // Adding new id fixed some weird issues
                        ImGui::PushID(passengers_vec.size() * 100 + i);
                        if (ImGui::BeginCombo("", buffer, 0))
                        {
                            for (int n = 0; n < stations_.size(); ++n)
                            {
                                sprintf(buffer, u8"Stacja %u", n);

                                const bool is_selected = (item_current_idx == n);
                                if (ImGui::Selectable(buffer, is_selected))
                                    item_current_idx = n;

                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::PopID();

                        // Column 2
                        ImGui::TableSetColumnIndex(1);
                        MakeInputPositive("", u8"Liczba pasażerów musi być dodatnia", &passengers_vec[i].count);

                        // Column 3
                        ImGui::TableSetColumnIndex(2);
                        if (ImGui::SmallButton(u8"Wyzeruj"))
                            passengers_vec[i].count = 0;


                        ImGui::PopID();
                    }

                    ImGui::EndTable(); // Inner Table
                }
            }
            ImGui::EndTable(); // Outer Table
        }
        ImGui::EndChild(); // End Table 3
    }

    ImGui::End(); // End Window
}
