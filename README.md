[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/76mHqLr5)
# Description

- Use `make` to build an executable, then run `./a.out` to start shell.
- Use `make clean` to clean up directory and remove a.out.

# Assumptions

- Newline Characters, ", ', ;,~ and & can be escaped using backslash or strings.
- ~ substitution must work for all arguments.
- Multiline input can be passed in the same way as in bash, using strings or \ .
- When there is no previous working directory, an error message can be printed
- While store pastevents, to handle multiline commands, null separated commands can be stored.
- In seek paths being printed will be relative to search directory, i.e to get actual path, will need to be concatenated
  with search directory
- pastevents execute will blindly substitute
- Edit makefile to efficiently compile files that changed only. 