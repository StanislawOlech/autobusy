#include "gui.hpp"

#include "imgui.h"
#include <charconv>


void GUI::Draw()
{
    ImGui::Begin("Okno");
    ImGui::BeginTabBar("Tabs");

    DrawAlgorithm();
    DrawStyle();

    ImGui::EndTabBar();
    ImGui::End();
}

void GUI::DrawStyle()
{
    if (ImGui::BeginTabItem("Styl"))
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
        ImGui::EndTabItem();
    }
}

void GUI::DrawAlgorithm()
{

    if (ImGui::BeginTabItem("Algorytm"))
    {
        if (ImGui::CollapsingHeader("Stacje"))
        {
            static int vec[2] = {0, 0};

            ImGui::InputInt2("Position", vec);
            if (ImGui::Button(u8"Dodaj nową stację"))
            {
                Point2D point2D = {vec[0], vec[1]};
                if (stations_.end() != std::find(stations_.begin(), stations_.end(), point2D)) [[unlikely]]
                {
                    ImGui::Text(u8"Stacja już istnieje");
                }
                else
                    stations_.emplace_back(vec[0], vec[1]);
            }

            /// Tabela z zapamiętanymi stacjami
            constexpr static ImGuiTableFlags table_flags = ImGuiTableFlags_ScrollY |
                                                           ImGuiTableFlags_RowBg |
                                                           ImGuiTableFlags_BordersOuter |
                                                           ImGuiTableFlags_BordersV |
                                                           ImGuiTableFlags_Resizable |
                                                           ImGuiTableFlags_Reorderable |
                                                           ImGuiTableFlags_Hideable;

            ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
            if (ImGui::BeginTable("Station_table", 4, table_flags, outer_size))
            {
                ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
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
                            stations_.erase(stations_.begin() + row);

                        ImGui::PopID();
                    }
                }
                ImGui::EndTable();
            }
        }
        if (ImGui::CollapsingHeader("Połączenia stacji"))
        {
            constexpr static ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit |
                                                           ImGuiTableFlags_ScrollX |
                                                           ImGuiTableFlags_ScrollY |
                                                           ImGuiTableFlags_BordersOuter |
                                                           ImGuiTableFlags_BordersInnerH |
                                                           ImGuiTableFlags_Hideable |
                                                           ImGuiTableFlags_HighlightHoveredColumn;


            if (ImGui::BeginTable("table_angled_headers", stations_.size() + 1, table_flags))
            {

                connections_.resize(stations_.size() * stations_.size());

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
                            ImGui::Checkbox("", (bool*)&connections_[row * stations_.size() + column]);
                            ImGui::PopID();
                        }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }

        ImGui::EndTabItem();
    }
}
