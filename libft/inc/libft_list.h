#ifndef LIST_H
# define LIST_H

typedef struct List {
	void        *content;
	struct List	*next;
}	List;

List	*ft_lstnew(void *content);
void	ft_lstadd_front(List **lst, List *new);
int		ft_lstsize(List *lst);
List	*ft_lstlast(List *lst);
void	ft_lstadd_back(List **lst, List *new);
void	ft_lstdelone(List *lst, void (*del)(void*));
void	ft_lstclear(List **lst, void (*del)(void*));
void	ft_lstiter(List *lst, void (*f)(void *));
List	*ft_lstmap(List *lst, void *(*f)(void *), void (*del)(void *));

#endif