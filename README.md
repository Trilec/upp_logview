# LogView Example (U++)

This is a minimal, self-contained U++ (Ultimate++) example that demonstrates a custom `LogView` control based on `RichEdit`.

## Features

- Creates a clean logging widget using U++ RichEdit control
- Demonstrates styled output (headers, success, errors) using QTF
- Thread-safe logging via `PostCallback`
- Shows how to encapsulate setup logic for reusability
- Uses modern U++ patterns (avoids `THISBACK`, supports `lambda` tasks)

## What's in the example

### `LogView` class
A facade-style wrapper around `RichEdit` with a small internal buffer for appending lines. Each line can be styled (normal, success, header, error), and log separation is supported with a horizontal rule.

### `ExampleRunner`
A demo application that shows how to use `LogView` to run a few fake tasks and log their outcomes in real time.

### Thread-Safety
All calls to `Log()` use `PostCallback` to ensure they run safely in the UI thread.

## Build Instructions

1. Open U++ TheIDE.
2. Import the `.upp` file as a new package.
3. Build & run. No extra dependencies required.

## Notes

This is an illustrative example only — not designed for high-volume or concurrent logging under load. For serious multi-threaded logging, use a proper producer/consumer pattern or `LogPos` with a background flush thread.

Licensed under MIT (or public domain — feel free to reuse).
