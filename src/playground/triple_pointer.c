/* ************************************************************************** */
/*   triple_pointer_simple.c - Understanding char *** in 50 lines            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ❌ WRONG: Can't modify the array pointer
void wrong_add_env(char **envp, char *new_var)
{
	(void)new_var;
	envp = malloc(10 * sizeof(char *));  // Only changes local copy!
	printf("  ❌ Inside: envp = %p (local copy changed)\n", (void *)envp);
	free(envp);
}

// ✅ RIGHT: Uses *** to modify the original array pointer
void correct_add_env(char ***envp, char *new_var)
{
	int count = 0;
	while ((*envp)[count])  // Count existing vars
		count++;
	
	char **new_envp = malloc((count + 2) * sizeof(char *));
	for (int i = 0; i < count; i++)
		new_envp[i] = (*envp)[i];  // Copy old pointers
	new_envp[count] = strdup(new_var);
	new_envp[count + 1] = NULL;
	
	free(*envp);       // Free old array (not the strings!)
	*envp = new_envp;  // Update original pointer ✅
	printf("  ✅ Inside: *envp = %p (original changed)\n", (void *)*envp);
}

int main(void)
{
	printf("\n🎃 TRIPLE POINTER (char ***) - Simple Demo\n");
	printf("==========================================\n\n");
	
	char **env = malloc(3 * sizeof(char *));
	env[0] = strdup("USER=student");
	env[1] = strdup("HOME=/home/student");
	env[2] = NULL;
	
	printf("Initial: env = %p\n", (void *)env);
	for (int i = 0; env[i]; i++)
		printf("  env[%d] = %s\n", i, env[i]);
	
	printf("\n--- WRONG (char **) ---\n");
	wrong_add_env(env, "PATH=/usr/bin");
	printf("After:   env = %p ❌ Same!\n", (void *)env);
	
	printf("\n--- RIGHT (char ***) ---\n");
	correct_add_env(&env, "PATH=/usr/bin");
	printf("After:   env = %p ✅ Changed!\n\n", (void *)env);
	
	for (int i = 0; env[i]; i++)
		printf("  env[%d] = %s\n", i, env[i]);
	
	for (int i = 0; env[i]; i++)
		free(env[i]);
	free(env);
	return (0);
}

