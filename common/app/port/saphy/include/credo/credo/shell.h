#ifndef CREDO_SHELL_H
#define CREDO_SHELL_H

#include "credo/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CR_CMDLINE_SIZE 1024

/**
 * @ingroup ShellIntegration
 * readline function signature that is custom readline function
 * @param[in] prompt prompt string
 * @param[out] input input string
 * @return Error Code
 */
typedef CredoErrorCodes_t (*CredoReadLine_t)(const char* prompt, char input[CR_CMDLINE_SIZE]);

/**
 * @brief Set custom shell readline function
 * @ingroup ShellIntegration
 * @param[in] func readline function pointer
 */
CREDOAPI void cr_shell_set_readline(CredoReadLine_t func);

/**
 * @brief Get all available shell commands
 *
 * Iterate through all the available shell commands with an increasing index until NULL is provided.
 *
 * @note there may be some empty "" strings that should be ignored.
 *
 * @ingroup ExShell
 * @param[in] index command index
 * @return command name string (returns NULL on out of index)
 */
CREDOAPI const char* cr_shell_get_command_name(int index);

/**
 * @brief Run a single shell command
 * @ingroup ShellCommands
 * @param[in] slice slice handle, if NULL it uses currently selected slice
 * @param[in] cmdline command string
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_run_command(CredoSlice_t* slice, const char* cmdline);

/**
 * @brief Run a lua slash command
 *
 * Doesn't enter the shell so you can run this in multiple threads.
 *
 * A more powerful replacement to cr_slice_display_info.
 *
 * Use "commands" to see the list of available commands.
 * @note Do not add the `/` prefix to your command
 * @ingroup ShellCommands
 * @param slice slice to run
 * @param command multi line slash command string to run
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_run_lua_command(CredoSlice_t* slice, const char* command);

/**
 * @brief Shell
 * @ingroup ShellSpawn
 *
 * @note Prefer using @ref cr_shell_spawn_enhanced unless using custom shell readline / logger hooks
 *
 * @param[in] slices slices to spawn shell. Use NULL to use current slices
 * @param[in] slice_count how many slices to spawn shell. Pass 0 for current slices
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_spawn(CredoSlice_t* slices[], int slice_count);

/**
 * @brief Spawn shell with battery included tools
 *
 * - Tab completion
 * - Line editing
 * - Command History
 *
 * @note Uses the currently selected slices for spawning the shell.

 * @ingroup ShellSpawn
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_spawn_enhanced(void);

/**
 * @brief Run a shell socket server
 *
 * Using Unix domain sockets, this will spawn a server that can be accessed via the crcli executable.
 * This is mainly for debugging a daemon process version of the sdk-- especially before the shell is integrated into
 * your own shell tool. It currently has no locking protection, so it should NOT be used in production environments.
 *
 * Sockets are stored in the /tmp/credo-*.sock.
 * @note Requires that libcrcli is linked.
 *
 * @ingroup ShellSpawn
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_spawn_server(void);

/**
 * @brief set shell slices
 * @ingroup ShellConfig
 * @param[in] slices slices to set
 * @param[in] slice_count how many slices
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_set_slices(CredoSlice_t* slices[], int slice_count);

/**
 * @brief Set the shell logger callback
 *
 * Separate optional logger for when running a shell command or a slice operation is being performed from a shell.
 * Otherwise it will default to normal sdk logger.
 * @ingroup ShellIntegration
 * @param shell_log_cb shell log call back function
 * @return Error Code
 */
CREDOAPI CredoErrorCodes_t cr_shell_set_logger(CredoLog_t shell_log_cb);

#ifdef __cplusplus
}
#endif

#endif  // CREDO_SHELL_H
