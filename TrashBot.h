#ifndef TRASHBOT_H
#define TRASHBOT_H


class TrashBot {
  public:
    TrashBot(int id, int x, int y);
    void move(int dx, int dy);
    void reportPosition() const;
    int id;
    int x;
    int y;
};

#endif // TRASHBOT_H