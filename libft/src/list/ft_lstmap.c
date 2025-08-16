#include "libft.h"

List	*ft_lstmap(List *lst, void *(*f)(void *), void (*del)(void *)) {
	List	*l;
	List	*e;

	if (!lst) {
		return (0);
	}
	l = 0;
	while (lst) {
		e = ft_lstnew(f(lst->content));
		if (!e) {
			ft_lstclear(&e, del);
			return (0);
		}
		ft_lstadd_back(&l, e);
		lst = lst->next;
	}
	return (l);
}
