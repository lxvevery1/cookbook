// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <atomic>     // for atomic
#include <functional> // for ref, reference_wrapper, function
#include <stddef.h>   // for size_t
#include <string> // for string, basic_string, char_traits, operator+, to_string
#include <thread> // for sleep_for, thread
#include <vector> // for vector

#include "ftxui/component/component.hpp" // for Checkbox, Renderer, Horizontal, Vertical, Input, Menu, Radiobox, ResizableSplitLeft, Tab
#include "ftxui/component/component_base.hpp"    // for ComponentBase, Component
#include "ftxui/component/component_options.hpp" // for MenuOption, InputOption
#include "ftxui/component/event.hpp"             // for Event, Event::Custom
#include "ftxui/component/screen_interactive.hpp" // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"
// for text, color, operator|, bgcolor, filler, Element, vbox, size, hbox,
// separator, flex, window, graph, EQUAL, paragraph, WIDTH, hcenter, Elements,
// bold, vscroll_indicator, HEIGHT, flexbox, hflow, border, frame, flex_grow,
// gauge, paragraphAlignCenter, paragraphAlignJustify, paragraphAlignLeft,
// paragraphAlignRight, dim, spinner, LESS_THAN, center, yframe, GREATER_THAN
#include "../include/FileLoader.hpp"
#include "ftxui/screen/terminal.hpp" // for Size, Dimensions

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::Fullscreen();

    uint32_t shift = 0;

    // ---------------------------------------------------------------------------
    // Paragraph
    // ---------------------------------------------------------------------------
    auto make_box = [](size_t dimx, size_t dimy) {
        std::string title = std::to_string(dimx) + "x" + std::to_string(dimy);
        return window(text(title) | hcenter | bold,
                      text("content") | hcenter | dim) |
               size(WIDTH, EQUAL, dimx) | size(HEIGHT, EQUAL, dimy);
    };

    FileLoader fl;
    fl.text_from_file = fl.load_write_lines(fl.FILE_PATH);

    auto paragraph_renderer_center = Renderer([&] {
        std::string str = fl.text_from_file;
        return vbox({
                   window(text("Align left:"), paragraphAlignLeft(str)),
                   window(text("Align center:"), paragraphAlignCenter(str)),
                   window(text("Align right:"), paragraphAlignRight(str)),
                   window(text("Align justify:"), paragraphAlignJustify(str)),
                   window(text("Side by side"), hbox({
                                                    paragraph(str),
                                                    separator(),
                                                    paragraph(str),
                                                })),
                   window(text("Elements with different size:"),
                          flexbox({
                              make_box(10, 5),
                              make_box(9, 4),
                              make_box(8, 4),
                              make_box(6, 3),
                              make_box(10, 5),
                              make_box(9, 4),
                              make_box(8, 4),
                              make_box(6, 3),
                              make_box(10, 5),
                              make_box(9, 4),
                              make_box(8, 4),
                              make_box(6, 3),
                          })),
               }) |
               vscroll_indicator | yframe | flex;
    });

    auto filler_renderer_right = Renderer([] {
        return paragraph(
                   "<--- This vertical bar is resizable using the mouse") |
               center;
    });

    auto filler_renderer_left = Renderer([] {
        return paragraph(
                   "This vertical bar is resizable using the mouse --->") |
               center;
    });

    // ---------------------------------------------------------------------------
    // ResizableSplit
    // ---------------------------------------------------------------------------

    int paragraph_renderer_split_position = Terminal::Size().dimx / 2;

    auto container = Renderer([fl] { return ftxui::text("fl\tfl"); });

    int side_margin = 40;

    container =
        ResizableSplitLeft(filler_renderer_left, container, &side_margin);
    container =
        ResizableSplitRight(filler_renderer_right, container, &side_margin);

    auto paragraph_renderer_group_renderer =
        Renderer(container, [&] { return container->Render(); });

    // ---------------------------------------------------------------------------
    // Tabs
    // ---------------------------------------------------------------------------

    int tab_index = 0;
    std::vector<std::string> tab_entries = {};
    auto tab_selection =
        Menu(&tab_entries, &tab_index, MenuOption::HorizontalAnimated());
    auto tab_content = Container::Tab(
        {
            paragraph_renderer_group_renderer,
        },
        &tab_index);

    auto exit_button =
        Button("Exit", [&] { screen.Exit(); }, ButtonOption::Animated());

    auto main_container = Container::Vertical({
        Container::Horizontal({
            tab_selection,
            exit_button,
        }),
        tab_content,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            tab_content->Render() | flex,
        });
    });

    std::atomic<bool> refresh_ui_continue = true;
    std::thread refresh_ui([&] {
        while (refresh_ui_continue) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(0.05s);
            // The |shift| variable belong to the main thread.
            // `screen.Post(task)` will execute the update on the thread where
            // |screen| lives (e.g. the main thread). Using `screen.Post(task)`
            // is threadsafe.
            screen.Post([&] { shift++; });
            // After updating the state, request a new frame to be drawn. This
            // is done by simulating a new "custom" event to be handled.
            screen.Post(Event::Custom);
        }
    });

    screen.Loop(main_renderer);
    refresh_ui_continue = false;
    refresh_ui.join();

    return 0;
}
