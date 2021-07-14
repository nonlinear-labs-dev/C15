#include <Commands.h>
#include <Command.h>
#include <algorithm>
#include <FrameBuffer.h>
#include <Font.h>
#include <regex>
#include <tuple>

extern char* selfPath;

namespace
{
  class ClearCommand : public Command
  {
   public:
    virtual int getNumArguments() const override
    {
      return 0;
    }

    virtual void execute(char**, int) override
    {
      FrameBuffer::get().clear();
    }

    virtual std::string getHelp() const override
    {
      return "clear => entirely clears the framebuffer";
    }

    virtual std::string getName() const override
    {
      return "clear";
    }
  };

  class TextCommand : public Command
  {
   public:
    virtual int getNumArguments() const override
    {
      return 3;
    }

    virtual void execute(char** argv, int numArgs) override
    {
      if(numArgs > 2)
      {
        auto app = Gio::File::create_for_path(selfPath);
        auto parent = app->get_parent();
        auto parentPath = parent->get_path();
        auto resourcePath = parentPath + "/resources/";

        Font f(resourcePath + "Emphase-8-Regular.ttf", 8);
        FrameBuffer::get().setColor(FrameBuffer::C255);
        f.draw(argv[0], atoi(argv[1]), atoi(argv[2]));
      }
    }

    virtual std::string getHelp() const override
    {
      return "'foo bar' x y => writes 'foo bar' at position x/y";
    }

    virtual std::string getName() const override
    {
      return "text";
    }
  };

  class HorizontalLineCommand : public Command
  {
   public:
    virtual int getNumArguments() const override
    {
      return 3;
    }

    virtual void execute(char** argv, int numArgs) override
    {
      if(numArgs > 2)
        FrameBuffer::get().drawHorizontalLine(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }

    virtual std::string getHelp() const override
    {
      return "x y l => draws a horizontal line of 'l' pixels length at x/y";
    }

    virtual std::string getName() const override
    {
      return "horizontal-line";
    }
  };

  class VerticalLineCommand : public Command
  {
   public:
    virtual int getNumArguments() const override
    {
      return 3;
    }

    virtual void execute(char** argv, int numArgs) override
    {
      if(numArgs > 2)
        FrameBuffer::get().drawVerticalLine(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }

    virtual std::string getHelp() const override
    {
      return "x y l => draws a vertical line of 'l' pixels length at x/y";
    }

    virtual std::string getName() const override
    {
      return "vertical-line";
    }
  };

  class MultiLineTextCommand : public Command
  {
    enum class Alignment
    {
      Left,
      Center,
      Right
    };

    enum class Display
    {
      Boled,
      Soled,
    };

    struct TextDef
    {
      Glib::ustring text;
      Alignment alignment;
      Display display;
      int line;
    };

    constexpr static int lineHeight = 10;
    std::unique_ptr<Font> font;

   public:
    MultiLineTextCommand()
    {
      auto app = Gio::File::create_for_path(selfPath);
      auto parent = app->get_parent();
      auto parentPath = parent->get_path();
      auto resourcePath = parentPath + "/resources/";
      font = std::make_unique<Font>(resourcePath + "Emphase-8-Regular.ttf", 8);
      FrameBuffer::get().setColor(FrameBuffer::C255);
    }

    virtual int getNumArguments() const override
    {
      return 1;
    }

    virtual void execute(char** argv, int numArgs) override
    {
      FrameBuffer::get().clear();
      for(int i = 0; i < numArgs; i++)
        draw(parsePart(argv[i]));
    }

    void draw(const TextDef& def)
    {
      if(def.display == Display::Boled)
        drawOnBoled(def);
      else
        drawOnSoled(def);
    }

    void drawOnBoled(const TextDef& def)
    {
      int x = calcLeft(def.text, def.alignment, 256);
      int y = lineHeight * (def.line + 1);
      font->draw(def.text, x, y);
    }

    void drawOnSoled(const TextDef& def)
    {
      int x = calcLeft(def.text, def.alignment, 128);
      int y = lineHeight * (def.line + 1) + 64;
      font->draw(def.text, x, y);
    }

    int calcLeft(const Glib::ustring& text, Alignment align, int pixelsAvailable) const
    {
      if(align == Alignment::Left)
        return 0;

      auto width = static_cast<int>(font->getStringWidth(text));

      if(align == Alignment::Right)
        return std::max(0, pixelsAvailable - width);

      auto diff = (pixelsAvailable - width) / 2;
      return std::max(0, diff);
    }

    TextDef parsePart(const std::string& in)
    {
      std::regex textAndFormatSplit("(.*)@(([sb012345lcr])*)$", std::regex_constants::icase);
      std::smatch match;

      if(std::regex_match(in, match, textAndFormatSplit))
        if(match.size() >= 2)
          return { match[1].str(), parseAlignment(match[2]), parseDisplay(match[2]), parseLine(match[2]) };

      return { in, Alignment::Center, Display::Soled, 1 };
    }

    Alignment parseAlignment(const std::string& in)
    {
      std::regex alignmentRegex(".*([lcr]).*", std::regex_constants::icase);
      std::smatch match;

      if(std::regex_match(in, match, alignmentRegex))
      {
        if(match.size() == 2)
        {
          auto alignemnt = match[1].str();
          if(alignemnt == "L" || alignemnt == "l")
            return Alignment::Left;
          if(alignemnt == "R" || alignemnt == "r")
            return Alignment::Right;
        }
      }
      return Alignment::Center;
    }

    Display parseDisplay(const std::string& in)
    {
      std::regex alignmentRegex(".*([bs]).*", std::regex_constants::icase);
      std::smatch match;

      if(std::regex_match(in, match, alignmentRegex))
      {
        if(match.size() == 2)
        {
          auto display = match[1].str();
          if(display == "B" || display == "b")
            return Display::Boled;
          if(display == "S" || display == "s")
            return Display::Soled;
        }
      }
      return Display::Soled;
    }

    int parseLine(const std::string& in)
    {
      std::regex alignmentRegex(".*([012345]).*", std::regex_constants::icase);
      std::smatch match;

      if(std::regex_match(in, match, alignmentRegex))
        if(match.size() == 2)
          return stoi(match[1].str());

      return 1;
    }

    virtual std::string getHelp() const override
    {
      return "text2soled multitext \"Text1@BL0\" \"Text2@SR1\" ... will draw Text1 on boled, left, Line 0 and Text2 on "
             "soled, right, line 1 ";
    }

    virtual std::string getName() const override
    {
      return "multitext";
    }
  };
}

Commands::Commands()
{
  m_commands.push_back(new ClearCommand());
  m_commands.push_back(new TextCommand());
  m_commands.push_back(new HorizontalLineCommand());
  m_commands.push_back(new VerticalLineCommand());
  m_commands.push_back(new MultiLineTextCommand());
}

Commands::~Commands()
{
}

void Commands::executeCommand(Command* c, int numArgs, char** argv)
{
  numArgs--;
  argv++;
  if(numArgs >= c->getNumArguments())
    c->execute(argv, numArgs);
  else
    showHelp();
}

void Commands::execute(int numArgs, char** argv)
{
  try
  {
    if(numArgs > 0)
    {
      if(auto c = findCommand(*argv))
      {
        executeCommand(c, numArgs, argv);
      }
      else
      {
        TextCommand text;
        if(numArgs >= text.getNumArguments())
          text.execute(argv, numArgs);
      }
    }
  }
  catch(...)
  {
    showHelp();
  }
}

Command* Commands::findCommand(const std::string& name) const
{
  auto c = std::find_if(m_commands.begin(), m_commands.end(), [&](Command* c) { return c->getName() == name; });

  if(c != m_commands.end())
    return *c;

  return nullptr;
}

void Commands::showHelp()
{
  for(auto c : m_commands)
  {
    std::cout << c->getName() << std::endl;
    std::cout << '\t' << c->getHelp() << std::endl << std::endl;
  }
}
