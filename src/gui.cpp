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
    if (ImGui::CollapsingHeader("Stacje"))
        DrawStationList();
    if (ImGui::CollapsingHeader("Połączenia stacji"))
        DrawStationTable();
    if (ImGui::CollapsingHeader(u8"Parametry sterujące"))
        DrawArguments();
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
    if (ImGui::BeginPopup("Uwaga"))
    {
        ImGui::Text(u8"Stacja już istnieje");
        if (ImGui::Button("Ok"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

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
    if (ImGui::InputInt("Liczba iteracji", &iteration_number_))
    {
        if (iteration_number_ <= 0)
        {
            iteration_number_ = 1;
            ImGui::OpenPopup("Uwaga##iteracje");
        }
    }
    if (ImGui::InputInt("Liczba autobusów", &autobus_number))
    {
        if (autobus_number <= 0)
        {
            autobus_number = 1;
            ImGui::OpenPopup("Uwaga##autobusy");
        }
    }

    // Popups
    if (ImGui::BeginPopup("Uwaga##iteracje"))
    {
        ImGui::Text(u8"Liczba iteracji musi być dodatnia");
        if (ImGui::Button("Ok"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopup("Uwaga##autobusy"))
    {
        ImGui::Text(u8"Liczba autobusów musi być dodatnia");
        if (ImGui::Button("Ok"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
