#pragma once

#include <set>
#include <mutex>
#include <array>
#include <tuple>
#include <utility>
#include <variant>

#include "item.hpp"
#include "screens/base.hpp"

namespace bookwyrm::screen {

class multiselect_menu : public base {
public:
    explicit multiselect_menu(vector<core::item> const &items);

    void paint() override;
    void on_resize() override;
    void toggle_action() override;
    void move(move_direction dir) override;
    string footer_info() const override;
    string controls_legacy() const override;
    int scrollpercent() const override;

    /*
     * Make some space for a screen:item_details,
     * and return how much we scrolled and how
     * big the new screen will be.
     */
    const std::pair<int, int> compress();

    /* Take back the space lent to screen::item_details */
    void decompress(int scroll);

    const core::item& selected_item() const
    {
        return items_[selected_item_];
    }

    size_t item_count() const
    {
        return items_.size();
    }

    auto marked_items()
    {
        return marked_items_;
    }

private:
    struct columns_t {

        struct column_t {
            using width_w_t = std::variant<int, double>;

            string title;

            /*
             * width_w(wanted).
             * How much space does the column want?
             * Can be specified as an absolute value or
             * as a multiplier, e.g. 0.30 for 30% of tb_width().
             */
            width_w_t width_w;

            /* Changes whenever the window dimensions are changed. */
            size_t width, startx;
        };

        /* Called upon menu construction. */
        void operator=(vector<std::pair<string, column_t::width_w_t>> &&pairs);

        column_t& operator[](const size_t i) { return columns_[i]; }
        size_t size() { return columns_.size();  }
        auto begin()  { return columns_.begin(); }
        auto end()    { return columns_.end();   }

    private:
        std::array<column_t, 6> columns_;
    };

    /* Store data about each column between updates. */
    columns_t columns_;

    /* Index of the currently selected item. */
    size_t selected_item_;

    /* How many lines have we scrolled? */
    size_t scroll_offset_;

    std::mutex menu_mutex_;
    vector<core::item> const &items_;

    /* Item indices marked for download. */
    std::set<int> marked_items_;

    bool is_marked(const size_t idx) const;

    /* How many entries can the menu print in the terminal? */
    size_t menu_capacity() const;

    /*
     * Is the currently selected item the last one in the
     * menu screen?
     */
    bool menu_at_bot() const;

    /*
     * Is the currently selected item the first one in the
     * menu screen?
     */
    bool menu_at_top() const;

    void mark_item(const size_t idx);
    void unmark_item(const size_t idx);

    void update_column_widths();

    void print_header();
    void print_column(const size_t col_idx);
};

} /* ns screen */
