#include "tetris_cli.hpp"

using namespace tetris;
using namespace tetris::cli;

tetris::cli::HelpFormatter::HelpFormatter(const std::string& run_command)
{
  usage =
    "Usage: " + run_command + " [OPTS]...";

  details =
    "-p, --preview-size SIZE  Set the number of tetriminoes to show in the piece preview." "\n"
    "    --disable-gravity    Pieces will not fall unless soft dropped or hard dropped, and" "\n"
    "                         must be hard dropped to lock in place." "\n"
    "\n"
    "-h                       Display brief help." "\n"
    "--help                   Display detailed help (i.e. this message).";

  brief =
    usage + "\n"
    + "Available opts: --preview_size (-p), --disable-gravity" "\n"
    + "Try '" + run_command + " --help' for more inforation.";

  complete =
    usage +  "\n"
    + "\n"
    + details;
}

opterror tetris::cli::process_options(int const argc, char* const argv[], control::GameSettings& settings)
{
  opterror rc = opterror_flag::NONE;

  int opt;
  int* longindex = nullptr;
  HelpFormatter help(argv[0]);
  while ((opt = getopt_long(argc, argv, cli::OPTSTRING, cli::LONGOPTS, longindex)) != -1)
  {
    switch(opt)
    {
      case 'p': // --preview-size
        settings.preview_size = atoi(optarg);
        break;

      case 256: // --disable-gravity
        settings.gravity = false;
        break;

      case 'h':
        std::cout << help.brief << std::endl;
        exit(0);
        break;

      case 1024: // --help
        std:: cout << help.complete << std::endl;
        exit(0);
        break;

      case '?':
        rc |= opterror_flag::BAD_OPT;
        break;

      default:
        break;
    }
  }

  if (settings.preview_size > 6)
  {
    std::cerr << "Error: "
              << "Maximum preview size is 6 (" << settings.preview_size << " attempted)."
              << std::endl;
    rc |= opterror_flag::BAD_ARG;
  }
  if (settings.preview_size < 0)
  {
    std::cerr << "Error: "
              << "Preview size must be non-negative (" << settings.preview_size << " attempted)."
              << std::endl;
    rc |= opterror_flag::BAD_ARG;
  }

  return rc;
}
