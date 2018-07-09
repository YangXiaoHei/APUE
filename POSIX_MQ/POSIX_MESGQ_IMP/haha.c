void InsertNode(Node *list, Node *newNode) {

    if (!list || !newNode)
        return;

    /* newNode 是最小的 */
    if (newNode->score < list->score) {
        /* 题目的 bug，无解 */
        return;
    }

    /* newNode 不是最小的 */

    /* 单节点情况 */
    if (!list->next) {
        list->next = newNode;
        newNode->next = NULL;
        return;
    }

    /* 多节点情况 */
    Node *prev = list, next = list->next;;
    while (next != NULL) {
        if (newNode->score <= next->score) {
            newNode->next = next;
            prev->next = newNode;
            return;
        } 
        next = next->next;
        prev = next;
    }

    if (next == NULL) {
        prev->next = newNode;
        newNode->next = NULL;
    }
}