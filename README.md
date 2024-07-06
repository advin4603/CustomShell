# Custom Shell
A custom shell like Bash using C with support for various shell operations, built as part of the OSN 2023 Course. For more details visit the [course page](https://karthikv1392.github.io/cs3301_osn/mini-projects/mp1)

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
