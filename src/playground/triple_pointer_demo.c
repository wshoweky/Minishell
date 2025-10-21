#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void	chang_int_wrong(int i)
{
	i += 13;
}

void	change_int_right(int *i)
{
	*i += 13;
}

void	update_env(char ***env, char *new)
{
	int	count = 0;
	while ((*env)[count])
		count++;
	char **new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
		return ;
	for (int i = 0; i < count; i++)
		new_env[i] = (*env)[i];
	new_env[count] = strdup(new);
	new_env[count + 1] = NULL;

	free(*env);
	(*env) = new_env;

}

int	main(void)
{
	int	i;

	i = 1337;
	chang_int_wrong(i);
	printf("wrong i = %d\n", i); // changes didn't take effect here!
	change_int_right(&i); 		// int must be changes by refference!
	printf("right i = %d\n", i);

	// env array ptr
	char	**env = (char **)malloc(sizeof(char *) * 3);
	if (!env)
		return (-1337);
	// strdup returns should be freed
	env[0] = strdup("USER");
	env[1] = strdup("HOME");
	env[2] = NULL;

	// show the old env pointer address
	printf("init env ptr = %p\n", (void *)env);

	// update the env ptr by reference! ***
	update_env(&env, "PATH");

	// print the updated pointer address
	printf("new env updated ptr = %p\n", (void *)env);

	// print the new env values
	for (int i = 0; env[i]; i++)
		printf("env[%d] = %s\n", i, env[i]);

	//free the strings
	for (int i = 0; env[i]; i++)
		free(env[i]);
	// free the array
	free(env);

	return (0);
}
