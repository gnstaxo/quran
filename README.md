Read the Word of God from your terminal

## Usage

usage: quran [flags] [reference...]

Flags:
  -A num  show num ayat of context after matching ayat
  -B num  show num ayat of context before matching ayat
  -C      show matching ayat in context of the surah
  -e      highlighting of surah and ayah numbers
          (default when output is a TTY)
  -p      output to less with surah grouping, spacing, indentation,
          and line wrapping
          (default when output is a TTY)
  -l      list suwar
  -h      show help

Reference:
    <Surah>
        Individual surah
    <Surah>:<Ayah>
        Individual ayah of a surah
    <Surah>:<Ayah>[,<Ayah>]...
        Individual ayat of a specific surah
    <Surah>:<Ayah>-<Ayah>
        Range of ayat in a surah

    /<Search>
        All ayat that match a pattern
    <Surah>/<Search>
        All ayat in a surah that match a pattern

## License

Public domain
