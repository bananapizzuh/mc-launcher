# mc-launcher
This is a library/command line tool for launching minecraft. I noticed there wasn't a library to launch minecraft with c/c++, so I created this. This is relatively my first go at using c, so the code quality will most likely be bad. Leave an issue or email me at [jay@bananapizzuh.dev](mailto:jay@bananapizzuh.dev) if you have a request or encounter a bug.

## Caveats
There is not an authentication scheme set up for the time being. For now, we are using the authentication json file from prism launcher/multimc.

## Usage
Currently, there is no documentation, and that may or may not change.

## Dependencies
- libcurl: https://repology.org/project/curl/versions
- cjson: https://repology.org/project/cjson/versions

# Compiling
`make` for the cli and the library.