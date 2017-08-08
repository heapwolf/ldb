#include <string>
#include "../ldb.h"

using namespace std;

string ldb::JSON(string source, int w = 2) {
  string s;

  bool quoted = false;
  bool escaped = false;
  string pad = string(w, ' ');
  int level = 0;
  int length = (int) source.length();
  int i;

  for (i = 0 ; i < length ; i++) {
    char ch = source[i];

    switch (ch) {
      case '{':
      case '[':
        s += ch;

        if (!quoted) {
          s += "\n";

          if (!(source[i + 1] == '}' || source[i + 1] == ']')) {
            ++level;

            for (int j = 0; j < level; j++) {
              s += pad;
            }
          }
        }

        break;

      case '}':
      case ']':
        if (!quoted) {
          if ((i > 0) &&
            (!(source[i - 1] == '{' || source[i - 1] == '['))) {
            s += "\n";

            --level;

            for (int j = 0 ; j < level ; j++) {
              s += pad;
            }
          } else if ((i > 0) && ((source[i - 1] == '[' && ch == ']') ||
            (source[i - 1] == '{' && ch == '}'))) {
            for (int j = 0 ; j < level ; j++) {
              s += pad;
            }
          }
        }

        s += ch;
        break;

      case '"':
        if (!quoted) {
          s += COLOR_BLUE;
          s += ch;
        } else {
          s += ch;
          s += COLOR_NONE;
        }

        escaped = false;

        if (i > 0 && source[i-1] == '\\') {
          escaped = !escaped;
        }

        if (!escaped) {
          quoted = !quoted;
        }

        break;

      case ',':
        s += ch;

        if (!quoted) {
          s += "\n";

          for (int j = 0 ; j < level ; j++) {
            s += pad;
          }
        }

        break;

      case ':':
        s += ch;

        if (!quoted) {
          s += " ";
        }

        break;

      default:
        s += ch;
        break;
    }
  }

  return s;
}
