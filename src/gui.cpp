/// UWAGA: plik przygotowany zgodnie z demo imgui, licencja MIT


#include "gui.hpp"

#include "imgui.h"
#include "implot.h"
#include <cmath>
#include <charconv>
#include <fstream>
#include <sstream>


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

bool CheckFileExists(char* file_name)
{
    std::ifstream file{file_name};
    return file.good();
}

void GUI::Draw()
{
    ImGui::Begin("Ustawienia");
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
    DrawResultPlot();
    ImGui::End();

    ImGui::Begin("Wynik");
    DrawResultWindow();
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
    {
        DrawArguments();

        ImGui::Spacing();

        ImGui::PushItemWidth(180);
        ImGui::InputText("Nazwa pliku do zapisu/wczytania", file_name, 32);
        ImGui::PopItemWidth();

        if (ImGui::Button(u8"Zapisz dane do plik"))
        {
            if (!CheckFileExists(file_name))
                SaveDataToFile();
            else
            {
                ImGui::OpenPopup(u8"Nadpisanie pliku");
            }
        }

        ImGui::SameLine();

        if (ImGui::Button(u8"Wczytaj dane z plik"))
        {
            try
            {
                LoadDataFromFile();
            }
            catch (const std::exception &e)
            {
                ImGui::OpenPopup(u8"Wczytywanie");
            }
        }

        if (ImGui::BeginPopupModal(u8"Nadpisanie pliku", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"Plik już istnieje. Czy chesz go nadpisać ?");

            if (ImGui::Button(u8"Tak"))
            {
                SaveDataToFile();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"Nie"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal(u8"Wczytywanie", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"Nie udało się poprawnie wczytać danych!");

            if (ImGui::Button(u8"Ok"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    if (open_passengers)
        DrawPassengers(&open_passengers);
}

void GUI::DrawResultPlot()
{
    constexpr static ImPlotAxisFlags xflags = ImPlotAxisFlags_None;
    constexpr static ImPlotAxisFlags yflags = ImPlotAxisFlags_None;
//    constexpr static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;


    ImPlotStyle &style = ImPlot::GetStyle();
    style.LineWeight = 2;

    if (ImPlot::BeginPlot("Wykres funkcji celu", {-1, -1})) {
        ImPlot::SetupAxes("Numer iteracji","Funkcja celu", xflags, yflags);


        if(y_value_.empty())
            ImPlot::SetupAxesLimits(-1, 1, -1, 1);
        else
        {
            ImPlot::SetupAxesLimits(-1, (int)y_value_.size() + 10, -10, *y_value_.rbegin() + 10, axis_flag);
            axis_flag = ImPlotCond_None;
        }

//        ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL, 1.5);
        ImPlot::PlotLine("f(x)", y_value_.data(), (int)y_value_.size());
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
        if (stations_.cend() != std::find(stations_.cbegin(), stations_.cend(), point2D)) [[unlikely]]
        {
            ImGui::OpenPopup("Uwaga");
        }
        else
        {
            stations_.emplace_back(vec[0], vec[1]);

            // Adding connections

//            auto print_conn = [&]() {
//                for (int row = 0; row < stations_.size(); ++row)
//                {
//                    for (int col = 0; col < stations_.size(); ++col)
//                        std::cout << (bool)connections_[row * stations_.size() + col] << " ";
//                    std::cout << "\n";
//                }
//                std::cout << "\n";
//            };
//
//            print_conn();
//            for (int i = connections_.size(); i >= 0; i -= stations_.size())
//            {
//                connections_.insert(connections_.begin() + i + 1, 0);
//                print_conn();
//            }
//
//            for (int i = 0; i < stations_.size(); ++i)
//                connections_.push_back(0);
//            print_conn();

        }
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
                    connections_.clear();

                    table3D.clear();

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
        // FIXME - manualnie do dodawania stacji
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

            // FIXME
            for (int column = 1; column < stations_.size() + 1; column++)
            {
                if (ImGui::TableSetColumnIndex(column) && column - 1 != row)
                {
                    std::size_t id = row * stations_.size() + column-1;

                    if (row > column - 1)
                        id = (column-1) * stations_.size() + row;

                    ImGui::PushID(id);
                    ImGui::Checkbox("", (bool*)&connections_[id]);
                    ImGui::PopID();
                }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUI::DrawArguments()
{
    ImGui::PushItemWidth(120);

    MakeInputPositive(u8"Liczba autobusów", u8"Liczba autbusów musi być dodatnia", &autobus_number_);
    MakeInputPositive(u8"Czas symulacji", u8"Czas symulacji musi być dodatni", &simulated_time_);

    MakeInputPositive(u8"Współczynnik straty pasażerów", u8"Współczynnik musi być dodatni", &passenger_loss_rate_);
    // TODO - dodać jakieś wyjaśnienie

    MakeInputPositive(u8"Liczba iteracji", u8"Liczba iteracji musi być dodatnia", &max_iter_);

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


    MakeInputPositive(u8"Liczba pszczół wysłanych do najlepszych rozwiązań",
                      u8"Liczba zrekrutowanych pszczół musi być dodatnia", &best_size_);

    if (ImGui::InputInt("Liczba pszczół wysłanych do elitarnych rozwiązań", &elite_size_))
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
                     u8"Otoczenie elitarnych rozwiązań musi być większe od otoczenia najlepszych rozwiązań");


    MakeInputPositive(u8"Rozmiar otoczenia rozwiązania", u8"Rozmiar otoczenia musi być dodatni", &neighborhood_size_);


    MakeInputPositive(u8"Czas życia rozwiązania", u8"Czas życia musi być dodatni", &lifetime_);



    if (ImGui::InputInt("Adaptacyjny rozmiar", &adaptive_size_))
    {
        if (adaptive_size_ > lifetime_)
        {
            adaptive_size_ = lifetime_;
        }
        else if (adaptive_size_ < 0)
            adaptive_size_ = 0;
    }
    ImGui::SameLine();

    HelpMarker("Po ilu iteracjach bez poprawy funkcji celu zminiejszenie\n o połowę rozmiaru sąsiectwa"
               " dla najlepszych rozwiązań, nie elitranych.\n"
               "Liczba zero oznacza brak adaptacyjnej zmiany");


    ImGui::PopItemWidth();


    ImGui::PushItemWidth(155);

    auto preview_text = CriterionToString(problem_criterion_);

    if (ImGui::BeginCombo(u8"Funkcja celu", preview_text.data()))
    {
        for (int i = 0; i < CRITERION_NR_ITEMS; ++i)
        {
            auto text = CriterionToString(static_cast<criterion>(i));
            const bool is_selected = (problem_criterion_ == i);

            if (ImGui::Selectable(text.data(), is_selected))
                problem_criterion_ = static_cast<criterion>(i);

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200);
    // TODO - dodać nowe otoczenia
    preview_text = NeighborhoodToString(neighborhood_);
    if (ImGui::BeginCombo(u8"Otoczenie", preview_text.data()))
    {
        for (int i = 0; i < NEIGHBORHOOD_NR_ITEMS; ++i)
        {
            auto text = NeighborhoodToString(static_cast<Neighborhood>(i));
            const bool is_selected = (neighborhood_ == i);

            if (ImGui::Selectable(text.data(), is_selected))
                neighborhood_ = static_cast<Neighborhood>(i);

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }


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

    static int selected_station_id = 0;
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

void GUI::DrawResultWindow()
{
    static decltype(std::chrono::high_resolution_clock::now()) start_time;
    static decltype(std::chrono::high_resolution_clock::now()) end_time;

    if (ImGui::Button("Uruchom algorytm"))
    {
        ImGui::OpenPopup("Algorytm");

        start_time = std::chrono::high_resolution_clock::now();
        future_bees = std::async(std::launch::async, &RunAlgorithm, ExportAlgorithm(), ExportProblem());
    }

    ImGui::Text(u8"Poprzedni czas wykonania: %.3f sekund", execution_time_ms_);

    ImGui::Text(u8"Liczba wyliczeń funkcji celu: %d", objectiveFunCalls_);


    double value = 0;
    if (!y_value_.empty())
        value = *y_value_.rbegin();
    ImGui::Text(u8"Najlepszy wynik: %.2lf", value);

    ImGui::Text(u8"Najlepsze rozwiązanie:");

    ImGui::Indent();
    ImGui::Text(best_solution_text_.c_str());
    ImGui::Unindent();

    /// Execution popup window
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Algorytm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        end_time = std::chrono::high_resolution_clock::now();

        ImGui::Text(u8"Proszę czekać");
        ImGui::Text(u8"Upłyneło: %d sekund", std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time));

        if (future_bees.valid())
        {
            if (future_bees.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                Bees bees = future_bees.get(); // it moves value

                y_value_ = bees.getResultIteration();
                objectiveFunCalls_ = bees.getObjectiveFunCalls();
                best_solution_text_ = bees.getBestBee().trams.Print();
                execution_time_ms_ = (float)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.f;
                axis_flag = ImPlotCond_Always;

                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}

AlgorithmParameters GUI::ExportAlgorithm() const
{
    return {
        .solutionsNumber = solutions_number_,
        .bestCount = best_number_,
        .eliteCount = elite_number_,
        .neighborhoodSize = neighborhood_size_,
        .maxIterations = max_iter_,
        .beeLifeTime = lifetime_,
        .adaptive_size = adaptive_size_
    };
}

ProblemParameters GUI::ExportProblem() const
{
    Graph<Point2D> graph = ConvertGuiGraphToGraph(connections_, stations_);

    PassengerTable::Table3D passengerTable = ConvertGuiTableToTable3D(table3D, stations_);

    StationList stationList{passengerTable, static_cast<uint32_t>(passenger_loss_rate_)};

    for (auto point2d : stations_)
        stationList.Create(point2d);

    TramProblem tramProblem{simulated_time_, stationList};

    return {
        autobus_number_,
        graph,
        tramProblem,
        stationList,
        problem_criterion_,
        neighborhood_
    };
}


void GUI::SaveDataToFile()
{

    std::ofstream file{file_name, std::ofstream::out};

    if (!file.is_open())
        ImGui::OpenPopup(u8"Nieudany zapis");
    else
    {
        file << "# Dane algorytmu\n";
        file << u8"Liczba autobusów: "  << autobus_number_ << '\n';
        file << u8"Czas symulacji: "  << simulated_time_ << '\n';
        file << u8"Współczynnik straty autobusów: " << passenger_loss_rate_ << '\n';

        file << u8"Liczba iteracji: " << max_iter_ << '\n';
        file << u8"Liczba rozwiązań: " << solutions_number_ << '\n';

        file << u8"Liczba rozwiązań najlepszych: " << best_number_ << '\n';
        file << u8"Liczba rozwiązań elitarnych: " << elite_number_ << '\n';

        file << u8"Liczba zatrudnionych pszczół do obszaru najlepszego: " << best_size_ << '\n';
        file << u8"Liczba zatrudnionych pszczół do obszaru elitarnego: " << elite_size_ << '\n';

        file << u8"Rozmiar sąsiectwa: " << neighborhood_size_ << '\n';
        file << u8"Czas życia rozwiązania: " << lifetime_ << '\n';
        file << u8"Adaptacyjny rozmiar: " << adaptive_size_ << '\n';
        file << u8"Funkcja celu: " << problem_criterion_ << '\n';
        file << u8"Rodzaj otoczenia: " << neighborhood_ << '\n';

        
        file << "\n# Stacje\n";
        for (auto point2D : stations_)
        {
            file << point2D.x << " " << point2D.y << "\n";
        }

        file << u8"\n# Połączenia\n";
        for (int row = 0; row < stations_.size(); row++)
        {
            for (int column = 0; column < stations_.size(); column++)
            {
                std::size_t id = row * stations_.size() + column;

                if (row > column)
                    id = column * stations_.size() + row;

                if (row == column || !connections_[id])
                    file << "0 ";
                else
                    file << "1 ";
            }
            file << "\n";
        }

        file << u8"\n# Lista pasażerów\n";
        for (auto& [idx, vec2d]: table3D)
        {
            file << idx << "=";
            for (auto &vec : vec2d)
            {
                for (auto passenger : vec)
                {
                    file << passenger.count << "-" << passenger.dest_id << " ";
                }
                file << "|";
            }
            file << "\n";
        }
        file.flush();
    }

    if (ImGui::BeginPopupModal("Nieudany zapis", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(u8"Nie można otworzyć pliku do zapisu");
        ImGui::Separator();
        if (ImGui::Button("Ok", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void GUI::LoadDataFromFile()
{
    std::ifstream file{file_name};

    if (!file.is_open())
        ImGui::OpenPopup(u8"Nieudany odczyt");

    std::string line{};

    auto get_number = [&]() {
        // skip empty line, skip comment line
        while (std::getline(file, line) && (line.empty() || line.starts_with('#')))
            ;

        std::size_t pos = line.find(": ");
        return std::abs(std::stoi(line.substr(pos + 2)));
    };

    autobus_number_ = get_number();
    simulated_time_ = get_number();
    passenger_loss_rate_ = get_number();
    max_iter_ = get_number();
    solutions_number_ = get_number();
    best_number_ = get_number();
    elite_number_ = get_number();
    best_size_ = get_number();
    elite_size_ = get_number();
    neighborhood_size_ = get_number();
    lifetime_ = get_number();
    adaptive_size_ = get_number();

    int i = get_number();
    if (i >= 0 && i < CRITERION_NR_ITEMS)
        problem_criterion_ = static_cast<criterion>(i);
    else
        problem_criterion_ = static_cast<criterion>(0);

    i = get_number();
    if (i >= 0 && i < NEIGHBORHOOD_NR_ITEMS)
        neighborhood_ = static_cast<Neighborhood>(i);
    else
        neighborhood_ = static_cast<Neighborhood>(0);

    std::getline(file, line);
    // skip empty line, skip comment line
    while (line.empty() || line.starts_with('#'))
        std::getline(file, line);

    // Load stations
    stations_.clear();
    int x, y;
    while (!line.empty())
    {
        std::size_t pos = line.find(' ');

        x = std::stoi(line.substr(0, pos));
        y = std::stoi(line.substr(pos + 1));

        stations_.emplace_back(x, y);
        std::getline(file, line);
    }

    // Load connections
    while (line.empty() || line.starts_with('#'))
        std::getline(file, line);

    connections_.clear();
    while (!line.empty())
    {
        std::string value{};
        std::istringstream iss{line};
        while (std::getline(iss, value, ' '))
        {
            x = std::stoi(value);
            connections_.push_back(x);
        }
        std::getline(file, line);
    }


    // Load passengers group
    while (line.empty() || line.starts_with('#'))
        std::getline(file, line);

    table3D.clear();
    std::string vec2d{};
    std::string vec{};
    std::string passenger{};
    std::size_t idx{};

    while (!line.empty())
    {
        auto idx_pos = line.find('=');
        idx = std::abs(std::stoi(line.substr(0, idx_pos)));

        std::istringstream iss{line.substr(idx_pos + 1)};
        while (std::getline(iss, vec2d, '|'))
        {
            std::vector<PassengerGui> vecPassengers{};

            std::istringstream iss2{vec2d};
            while (std::getline(iss2, vec, ' '))
            {
                auto pos2 = vec.find('-');
                int count = std::stoi(vec.substr(0, pos2));
                int dest = std::stoi(vec.substr(pos2 + 1));

                vecPassengers.emplace_back(count, dest);
            }
            table3D[idx].push_back(std::move(vecPassengers));
        }
        std::getline(file, line);
    }

}


Bees RunAlgorithm(AlgorithmParameters algorithmParameters, ProblemParameters problemParameters)
{
    constexpr Point2D depot{0, 0};
    std::random_device bees_seed{};

    Bees bees(algorithmParameters, bees_seed(), depot, std::move(problemParameters));

    bees.run();
    return bees;
}


