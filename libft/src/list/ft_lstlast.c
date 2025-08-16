#include "libft.h"

List	*ft_lstlast(List *lst) {
	if (!lst) {
		return (0);
	}
	while (lst->next) {
		lst = lst->next;
	}
	return (lst);
}
