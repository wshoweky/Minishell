#!/usr/bin/bash

valgrind --trace-children=yes --track-fds=yes --leak-check=full --show-leak-kinds=all --gen-suppressions=all --suppressions=readline.supp --track-origins=yes ./minishell
