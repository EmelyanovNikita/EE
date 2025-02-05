#ifndef FLAGS_H
#define FLAGS_H

// определяем флаги для работы с типом ячейки - свободна, занята, удалена
typedef enum 
{
    FREE = 0,
    OCCUPIED = 1,
    DELETED = 2
} Flags;

#endif //FLAGS_H