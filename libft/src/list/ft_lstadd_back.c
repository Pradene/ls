#include "libft.h"

void	ft_lstadd_back(List **lst, List *new) {
	List	*p;

	if (lst) {
		if (*lst) {
			p = ft_lstlast(*lst);
			p->next = new;
		} else {
			*lst = new;
		}
	}
}
