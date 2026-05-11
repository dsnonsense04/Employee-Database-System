A simple yet effective way to store and manipulate employee data.

NOTE: This program may be faulty on Windows systems due to the use of std::rename() and std::remove() in the delete_records() function. 
      The problem seems to be that Linux can mess with files that already exist, but Windows cannot. I don't know a workaround, so it looks like the delete_records() function won't work on Windows.
