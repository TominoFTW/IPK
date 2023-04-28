__Limitations__
- TCP communication with truncated message without `'\n'` waits for the rest of the message from client. Doesn't disconects client until BYE message is received.
- Linux support only


__Changelog__\
Date:   Mon Apr 17 21:31:43 2023 +0200

    TCP Communication v2 multiquery support added

Date:   Mon Apr 17 21:30:39 2023 +0200

    Socket closing added

Date:   Mon Apr 17 02:55:33 2023 +0200

    Small changes

Date:   Mon Apr 17 01:52:23 2023 +0200

    TCP Communication DONE, multiline support needed

Date:   Sun Apr 16 17:58:21 2023 +0200

    Separeted into multiple files

Date:   Sun Apr 16 17:57:54 2023 +0200

    TCP Communication separeted
        - TCP not implemented

Date:   Sun Apr 16 17:52:58 2023 +0200

    UDP Communication v1
        - Doesn't support SIGINT

Date:   Sun Apr 16 17:52:09 2023 +0200

    Parsing of msg added

Date:   Sun Apr 16 17:50:32 2023 +0200

    Argument parsing added

Date:   Sun Apr 16 17:48:55 2023 +0200

    Makefile modified for multiple files

Date:   Fri Apr 14 02:37:53 2023 +0200

    Completed validation and calculation of msg

Date:   Tue Mar 28 18:43:13 2023 +0200

    Initial commit
