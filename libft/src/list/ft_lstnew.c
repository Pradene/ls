#include "libft.h"

List	*ft_lstnew(void *content) {
	List	*new;

	new = malloc(sizeof(List));
	if (!new) {
		return (0);
	}
	new->content = content;
	new->next = 0;
	return (new);
}
