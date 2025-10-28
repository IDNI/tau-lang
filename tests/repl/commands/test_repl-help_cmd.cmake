#
# help command
#

include(add_repl_test)

# basic help command
add_repl_test(help_cmd-full "help" "General")
add_repl_test(help_cmd-shortened "h" "General")

# help normalize command
add_repl_test(help_normalize_command_cmd-full "help normalize" "the normalize command")
add_repl_test(help_shortened_normalize_cmd "h normalize" "the normalize command")

# help qelim command
add_repl_test(help_qelim_command_cmd-full "help qelim" "the qelim command eliminates")
add_repl_test(help_shortened_qelim_cmd "h qelim" "the qelim command eliminates")

# help normalize command with shortened command
add_repl_test(help_normalize_cmd_shortened "help n" "the normalize command")
add_repl_test(help_shortened_normalize_cmd-shortened "h n" "the normalize command")

# help dnf
add_repl_test(help_shortened-dnf_cmd "h dnf" "dnf converts")
add_repl_test(help_dnf_cmd "help dnf" "dnf converts")

# help cnf command
add_repl_test(help_shortened-cnf_cmd "h cnf" "cnf converts")
add_repl_test(help_cnf_cmd "help cnf" "cnf converts")

# help nnf command
add_repl_test(help_shortened-nnf_cmd "h nnf" "nnf converts")
add_repl_test(help_nnf_cmd "help nnf" "nnf converts")

# help mnf command
add_repl_test(help_shortened-mnf_cmd "h mnf" "mnf converts")
add_repl_test(help_mnf_cmd "help mnf" "mnf converts")

# help onf command
add_repl_test(help_shortened-onf_cmd "h onf" "onf converts")
add_repl_test(help_onf_cmd "help onf" "onf converts")

# help history command
add_repl_test(help_shortened-history_cmd "h history" "the history")
add_repl_test(help_history_cmd "help history" "the history")
add_repl_test(help_shortened-history_shortened_m_cmd "h hist" "the history")
add_repl_test(help-history_shortened_m_cmd "help hist" "the history")
