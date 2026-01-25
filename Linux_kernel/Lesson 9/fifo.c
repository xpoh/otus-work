#include <stdio.h>
#include <stdlib.h>

/* Структура для узла очереди */
typedef struct Node {
	int data;
	struct Node *next;
} Node;

/* Структура для FIFO очереди */
typedef struct FIFO {
	/* Указатель на первый узел (начало очереди) */
	Node *start;
	/* Указатель на последний узел (конец очереди) */
	Node *end;
} FIFO;

/* Функция для выделения памяти под узел */
Node *alloc_node(int value)
{
	Node *new_node = (Node *)malloc(sizeof(Node));
	if (!new_node) {
		fprintf(stderr,
			"Ошибка: не удалось выделить память для узла.\n");
		exit(EXIT_FAILURE);
	}
	new_node->data = value;
	new_node->next = NULL;
	return new_node;
}

/* Функция для создания новой FIFO очереди */
FIFO *create_fifo()
{
	FIFO *q = (FIFO *)malloc(sizeof(FIFO));
	if (!q) {
		fprintf(stderr,
			"Ошибка: не удалось выделить память для очереди.\n");
		exit(EXIT_FAILURE);
	}
	q->start = q->end = NULL;
	return q;
}

/* функция для проверки, пуста ли очередь */
static inline int fifo_is_empty(FIFO *q)
{
	return q->start == NULL;
}

/* Функция для удаления элемента из очереди */
int fifo_out(FIFO *q)
{
	int value;

	if (fifo_is_empty(q)) {
		fprintf(stderr,
			"Ошибка: очередь пуста, невозможно удалить элемент.\n");
		return -1;
	}

	/* Сохраняем указатель на первый узел */
	Node *tmp = q->start;
	/* Сохраняем данные для возврата */
	value = tmp->data;
	/* Перемещаем указатель на следующий узел */
	q->start = q->start->next;

	if (fifo_is_empty(q))
		q->end = NULL;

	free(tmp);
	return value;
}

// Функция для добавления элемента в очередь (fifo_in)
void fifo_in(FIFO *q, int value)
{
	Node *tmp;

	tmp = alloc_node(value);

	if (!q->end) {
		/* Новый узел становится и началом, и концом очереди */
		q->start = q->end = tmp;
		return;
	}

	/* Добавляем новый узел в конец очереди */
	q->end->next = tmp;
	/* Обновляем указатель на конец очереди */
	q->end = tmp;
}

int main()
{
	FIFO *q = create_fifo();

	/* Добавляем элементы в очередь */
	fifo_in(q, 10);
	fifo_in(q, 20);
	fifo_in(q, 30);

	/* Удаляем элемент и выводим его */
	printf("%d out\n", fifo_out(q));
	printf("%d out\n", fifo_out(q));

	if (fifo_is_empty(q))
		printf("fifo is empty\n");
	else
		printf("fifo is not empty\n");

	/* Освобождение оставшихся узлов */
	for (; !fifo_is_empty(q);)
		fifo_out(q);

	free(q);

	exit(EXIT_SUCCESS);
}
