__Limitations__
- Windows is not supported
- TCP is limited to 1022 characters
- UDP is limited to 255 characters


__CHANGELOG__

Date:   Tue Mar 21 18:12:16 2023 +0100

    fixed C-c printing
    - C-c is not printing any message (works acording to forum)


Date:   Tue Mar 21 17:47:15 2023 +0100

    added Timeout for UDP
    - UDP now has a timeout of 5 seconds


Date:   Tue Mar 21 01:08:32 2023 +0100

    added message validating
    - validating message length is now fixly set to 1022 for TCP and 255 for UDP without causing errors or issues


Date:   Tue Mar 21 00:42:18 2023 +0100

    fixed typos and added EOF handling
    - EOF is now handled as BYE in TCP and closes the connection


Date:   Sun Mar 19 14:11:03 2023 +0100

    added comments


Date:   Sun Mar 19 13:48:21 2023 +0100

    added C-c handling


Date:   Sun Mar 19 00:11:01 2023 +0100

    added argument parsing
    - order of arguments does not matter, but they must be present


Date:   Sat Mar 18 23:04:46 2023 +0100

    added loops and errors
    - first loops for continous input and basic error handling


Date:   Sat Mar 18 22:51:07 2023 +0100

    added code from stubs
    - added TCP, UDP and libraries
