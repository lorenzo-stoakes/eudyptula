  - Take the kernel module you wrote for task 01, and modify it to
    create a debugfs subdirectory called "eudyptula".  In that
    directory, create the virtual file called "id".
  - This "id" file, operates just like it did for example 06, and uses
    the same logic there, the file is to be readable and writable by any
    user.
  - Submit this task as the first email.
  - Base your work on this submission, and create a new debugfs file
    called "jiffies".
  - This "jiffies" file is to be read-only by any user, and when read,
    should return the current value of the jiffies kernel timer.
  - Submit this result as a patch against the first email.
  - Base your work again on that submission, and create a final debugfs
    file called "foo".
  - The file "foo" needs to be writable only by root, but readable by
    anyone.  When writing to it, the value must be stored, up to one
    page of data.  When read, which can be done by any user, the value
    that is stored in it must be returned.  Properly handle the fact
    that someone could be reading from the file while someone else is
    writing to it (oh, a locking hint!)
  - Submit this result as a patch against the second email.
  - When the module is unloaded, all of the debugfs files are cleaned
    up, and any memory allocated is freed, for all submissions.
  - Provide some "proof" this all works in the first email.
