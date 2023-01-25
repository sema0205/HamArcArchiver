# HamArc Archiver


Implementation of a fault-tolerant file archiver without compression **HamArc**, which combines multiple files into one archive. Use [Hamming Codes](https://en.wikipedia.org/wiki/Hamming_code) for fault-tolerant encoding.

## Features

* Combines multiple files ( >= 1) into one archive in .haf format (Hamming Archive File)
* Extracts all or individual files from the archive
* Combines multiple archives into one
* Restores the archive when damaged, or inform that this is impossible
* Returns a list of files in the archive

Console application supports the following command line arguments:

**-c, --create**           - create a new archive

**-f, --file=[ARHCNAME]**  - name of the archive file

**-l, --list**             - display a list of files in the archive

**-x, --extract**          - extract files from the archive (if not specified, all files)

**-a, --append**           - add a file to the archive

**-d, --delete**           - delete a file from the archive

**-A, --concatenate**      - merge two archives

**File names are passed as free arguments**

**Arguments for encoding and decoding are also passed through the command line**

### Examples of launch
   
*hamarc --create --file=ARCHIVE FILE1 FILE2 FILE3*
   
*hamarc -l -f ARCHIVE*
   
*hamarc --concantenate  ARCHIVE1 ARCHIVE2 -f ARCHIVE3*
