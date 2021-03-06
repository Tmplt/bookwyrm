#pragma once

#include <fmt/format.h>

#include "item.hpp"
#include "screens/base.hpp"

/*
 * Interface-wise, this will be like opening an email for reading in mutt.
 * A thread will be spawned to fetch more info about the item from some
 * database, and the bookwyrm will print that info in this window in a pretty
 * way.
 *
 * The user should still be able to check another item's details while this
 * thread is running. Item details will be kept in the actual item. So a passed
 * item will be modified, otherwise, if the user goes back to an item, we'll
 * need to fetch the data again.
 *
 * The user doesn't need to exit the detail screen to select another item for
 * details. Implementing this is a problem for the future, though.
 */
namespace bookwyrm::tui::screen {

    class item_details : public base {
    public:
        explicit item_details(const core::item &item, int padding_top);

        void paint() override;

        std::string controls_legacy() const override;

    private:
        const core::item &item_;

        void print_borders();
        void print_details();
    };

} // namespace bookwyrm::tui::screen
