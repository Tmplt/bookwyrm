#include <clocale>
#include "core/plugin_handler.hpp"
#include "core/item.hpp"
#include "utils.hpp"
#include "version.hpp"
#include "command_line.hpp"
#include "tui.hpp"
#include "downloader.hpp"

int main(int argc, char *argv[])
{
    std::setlocale(LC_ALL, "");

    const auto main = cligroup("Main", "necessarily inclusive arguments; at least one required")
        ("-a", "--author",     "Specify authors",   "AUTHOR")
        ("-t", "--title",      "Specify title",     "TITLE")
        ("-s", "--series",     "Specify series",     "SERIE")
        ("-p", "--publisher",  "Specify publisher", "PUBLISHER");

    const auto excl = cligroup("Exclusive", "cannot be combined with any other arguments")
        ("-d", "--ident",      "Specify an item identification (such as DOI, URL, etc.)", "IDENT");

    const auto exact = cligroup("Exact", "all are optional")
        ("-y", "--year",       "Specify year of release. "
                               "A prefix modifier can be used to broaden the search. "
                               "Available prefixes are <, >, <=, >=.", "YEAR")
        ("-L", "--language",   "Specify text language",  "LANG")
        ("-e", "--edition",    "Specify item edition",   "EDITION")
        ("-E", "--extension",  "Specify item extension", "EXT")
        ("-i", "--isbn",       "Specify item ISBN", "ISBN");

    const auto misc = cligroup("Miscellaneous")
        ("-h", "--help",       "Display this text and exit")
        ("-v", "--version",    "Print version information (" + build_info_short + ") and exit")
        ("-D", "--debug",      "Set logging level to debug");

    const cligroups groups = {main, excl, exact, misc};

    const auto cli = [=]() -> cliparser {
        string progname = argv[0];
        vector<string> args(argv + 1, argv + argc);

        auto cli = cliparser::make(std::move(progname), std::move(groups));

        try {
            cli.process_arguments(args);
        } catch (const argument_error &err) {
            fmt::print(stderr, "error: {}; see --help\n", err.what());

            // evil! Return optional instead?
            std::exit(EXIT_FAILURE);
        }

        return cli;
    }();

    if (cli.has("help")) {
        cli.usage();
        return EXIT_SUCCESS;
    } else if (cli.has("version")) {
        print_build_info();
        return EXIT_SUCCESS;
    } else if (argc == 1) {
        cli.usage();
        return EXIT_FAILURE;
    }

    try {
        cli.validate_arguments();
    } catch (const argument_error &err) {
        fmt::print(stderr, "error: {}; see --help\n", err.what());
        return EXIT_FAILURE;
    }

    const string dl_path = cli.has(0) ? cli.get(0) : ".";

    if (const auto err = bookwyrm::utils::validate_download_dir(dl_path); err) {
        string msg = err.message();
        std::transform(msg.begin(), msg.end(), msg.begin(), ::tolower);
        fmt::print(stderr, "error: invalid download directory: {}.\n", msg);
        return EXIT_FAILURE;
    }

    bookwyrm::downloader d(dl_path);
    vector<core::item> wanted_items;

    try {
        auto logger = std::make_shared<bookwyrm::logger>();
        logger->set_level(core::log_level::warn);

        if (cli.has("debug"))
            logger->set_level(core::log_level::debug);

        logger->debug("the mighty eldwyrm hath been summoned!");

        const core::item wanted = bookwyrm::utils::create_item(cli);
        auto butler = core::plugin_handler(std::move(wanted));

        /*
         * Find and load all worker scripts.
         * During run-time, the butler will match each found item
         * with the wanted one. If it doesn't match, it is discarded.
         */
        auto tui = bookwyrm::make_tui_with(butler, logger);

        if (tui->display()) {
            /*
             * Start download while plugin_handler destructs.
             * NOTE: the TUI is blocked here; we don't want that.
             */
            /* d.async_download(tui->get_wanted_items()); */
            wanted_items = tui->get_wanted_items();
        }

    } catch (const component_error &err) {
        fmt::print(stderr, "A dependency failed: {}. Developer error? Terminating...\n", err.what());
        return EXIT_FAILURE;
    } catch (const program_error &err) {
        fmt::print(stderr, "Fatal program error: {}; I can't continue! Terminating...\n", err.what());
        return EXIT_FAILURE;
    }

    if (wanted_items.empty()) {
        /* We have nothing else to do. */
        return EXIT_SUCCESS;
    }

    try {
        if (wanted_items.size() == 1)
            fmt::print("Downloading item...\n");
        else
            fmt::print("Downloading {} items...\n", wanted_items.size());

        auto success = d.sync_download(wanted_items);

        if (!success && wanted_items.size() > 1) {
            fmt::print("No items were successfully downloaded\n");
            return EXIT_FAILURE;
        }

    } catch (const component_error &err) {
        fmt::print(stderr, "Fatal program error: {}; I can't continue! Terminating...\n", err.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
