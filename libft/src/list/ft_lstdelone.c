#include "libft.h"

void	ft_lstdelone(List *lst, void (*del)(void *)) {
	if (!lst) {
		return ;
	}
	del(lst->content);
	free(lst);
}
