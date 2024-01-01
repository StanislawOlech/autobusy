#include "gui.hpp"

#include "imgui.h"
#include "implot.h"
#include <cmath>
#include <charconv>
#include <fstream>


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

        if (ImGui::Button(u8"Zapisz dane do plik"))
            SaveDataToFile();
    }



    if (open_passengers)
        DrawPassengers(&open_passengers);
}

void GUI::DrawResultPlot()
{
    static std::vector<double> x_value;
    x_value.resize(y_value_.size());
    std::iota(x_value.begin(), x_value.end(), 1);


    if (ImPlot::BeginPlot("Wykres funkcji celu", {-1, -1})) {
        ImPlot::SetupAxes("Numer iteracji","Funkcja celu");
        ImPlot::PlotLine("f(x)", x_value.data(), y_value_.data(), (int)y_value_.size());
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

    MakeInputPositive(u8"Liczba autobusów", u8"Liczba autbusów musi być dodatnia", &autobus_number_);
    MakeInputPositive(u8"Współczynnik straty pasażerów", u8"Współczynnik musi być dodatni", &passenger_loss_rate);
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




    ImGui::PopItemWidth();


    ImGui::PushItemWidth(145);

    auto preview_text = CriterionToString(problemCriterion);

    if (ImGui::BeginCombo(u8"Funkcja celu", preview_text.data()))
    {
        for (int i = 0; i < CRITERION_NR_ITEMS; ++i)
        {
            auto text = CriterionToString(static_cast<criterion>(i));
            const bool is_selected = (problemCriterion == i);

            if (ImGui::Selectable(text.data(), is_selected))
                problemCriterion = static_cast<criterion>(i);

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

void GUI::DrawResultWindow()
{
    static decltype(std::chrono::high_resolution_clock::now()) start_time;
    static decltype(std::chrono::high_resolution_clock::now()) end_time;

    if (ImGui::Button("Uruchom algorytm"))
    {
        ImGui::OpenPopup("Algorytm");

        start_time = std::chrono::high_resolution_clock::now();
        future_y_value_ = std::async(std::launch::async, &RunAlgorithm);
    }

    ImGui::Text(u8"Poprzedni czas wykonania: %d sekund", execution_time / 100 );


    /// Execution popup window
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Algorytm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        end_time = std::chrono::high_resolution_clock::now();

        ImGui::Text(u8"Proszę czekać");
        ImGui::Text(u8"Upłynięty czas: %d sekund", std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time));

        if (future_y_value_.valid())
        {
            if (future_y_value_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                y_value_ = future_y_value_.get();
                execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
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
        .beeLifeTime = lifetime_
    };
}

ProblemParameters GUI::ExportProblem() const
{
    Graph<Point2D> graph = ConvertGuiGraphToGraph(connections_, stations_);

//    TramProblem tramProblem{};
    PassengerTable::Table3D passengerTable = ConvertGuiTableToTable3D(table3D, stations_);
    // TODO finish
}

void GUI::SaveDataToFile()
{
    std::ofstream file{"Zapis.txt", std::ofstream::out};

    if (!file.is_open())
        ImGui::OpenPopup(u8"Nieudany zapis");
    else
    {
        file << "# Dane algorytmu\n";
        file << u8"Liczba autobusów: "  << autobus_number_ << '\n';
        file << u8"Współczynnik straty autobusów: " << passenger_loss_rate << '\n';

        file << u8"Liczba iteracji: " << max_iter_ << '\n';
        file << u8"Liczba rozwiązań: " << solutions_number_ << '\n';

        file << u8"Liczba rozwiązań najlepszych: " << best_number_ << '\n';
        file << u8"Liczba rozwiązań elitarnych: " << elite_number_ << '\n';

        file << u8"Liczba zatrudnionych pszczół do obszaru najlepszego: " << best_size_ << '\n';
        file << u8"Liczba zatrudnionych pszczół do obszaru elitarnego: " << elite_size_ << '\n';

        file << u8"Rozmiar sąsiectwa: " << neighborhood_size_ << '\n';
        file << u8"Czas życia rozwiązania: " << neighborhood_size_ << '\n';
        file << u8"Funkcja celu: " << problemCriterion << '\n';

    }

    if (ImGui::BeginPopupModal("Nieudany zapis", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(u8"Nie można otworzyć pliku do zapisu");
        ImGui::Separator();
        if (ImGui::Button("Ok", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

std::vector<double> RunAlgorithm()
{
    Point2D depot{0, 0};

    // generating graph and stations
    Graph<Point2D> graph{};
    PassengerTable::Table3D table3D;
    StationList stationList{table3D};

    for (int x = 0; x != n; x++){
        for (int y = 0; y != m; y++){
            if (y + 1 < m){
                graph.AddEdge({x, y}, {x, y + 1});
                graph.AddEdge({x, y + 1}, {x, y});
            }
            if (x + 1 < n){
                graph.AddEdge({x, y}, {x + 1, y});
                graph.AddEdge({x + 1, y}, {x, y});
            }
            if (x + 1 < n && y + 1 < m){
                graph.AddEdge({x, y}, {x + 1, y + 1});
                graph.AddEdge({x + 1, y + 1}, {x, y});
                graph.AddEdge({x + 1, y}, {x, y + 1});
                graph.AddEdge({x, y + 1}, {x + 1, y});
            }
            stationList.Create({x, y});
        }
    }

    // random trams generation
    TramList trams;
    trams.gen_rand_trams(graph, tram_amount, tram_length, depot);

    TramProblem tramProblem(time_itt, stationList);

    std::random_device bees_seed{};
    AlgorithmParameters algorithmParameters{};
    algorithmParameters.solutionsNumber  = 20;
    algorithmParameters.bestCount        = 10;
    algorithmParameters.eliteCount       =  5;
    algorithmParameters.bestRecruits     =  5;
    algorithmParameters.eliteRecruits    = 10;
    algorithmParameters.neighborhoodSize = 10;
    algorithmParameters.maxIterations    =100;
    algorithmParameters.beeLifeTime      = 10;

    ProblemParameters problemParameters(tram_amount, graph, tramProblem, stationList, max_transported);

    Bees bees(algorithmParameters, bees_seed(), depot, problemParameters);

    bees.run();

    return bees.getResultIteration();
}


