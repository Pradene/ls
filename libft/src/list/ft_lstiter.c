#include "libft.h"

void	ft_lstiter(List *lst, void (*f)(void *)) {
	if (!lst) {
		return ;
	}
	while (lst) {
		f(lst->content);
		lst = lst->next;
	}
}
