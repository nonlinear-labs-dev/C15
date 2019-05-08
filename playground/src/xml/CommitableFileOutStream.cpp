#include <xml/CommitableFileOutStream.h>

CommitableFileOutStream::CommitableFileOutStream(const Glib::ustring &fileName, bool zip)
    : super(fileName, zip)
{
  setAutoCommit(false);
}
