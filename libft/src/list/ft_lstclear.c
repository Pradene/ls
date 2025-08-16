#include "libft.h"

void	ft_lstclear(List **lst, void (*del)(void *)) {
	List	*p;

	if (lst && del) {
		p = *lst;
		while (*lst) {
			p = (*lst)->next;
			ft_lstdelone(*lst, del);
			*lst = p;
		}
	}
}
