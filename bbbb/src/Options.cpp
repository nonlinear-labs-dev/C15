#include "Options.h"
#include <glibmm/optiongroup.h>

Options::Options(int &argc, char **&argv)
{
  Glib::OptionGroup mainGroup("common", "common options");
  Glib::OptionContext ctx;

  Glib::OptionEntry pg;
  pg.set_long_name("playground-host");
  pg.set_short_name('p');
  pg.set_description("Where to find the playground");
  mainGroup.add_entry(pg, m_playgroundHost);

  ctx.set_main_group(mainGroup);
  ctx.set_help_enabled(true);

  ctx.parse(argc, argv);
}

Glib::ustring Options::getPlaygroundHost() const
{
  return m_playgroundHost;
}
