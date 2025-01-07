import numpy as np
import time
import os


def count_neighbors(grid):
    # Подсчёт количества соседей для каждой клетки
    
    # Сдвигаем матрицу в разные стороны и суммируем c помощью библиотеки numpy
    neighbors = (
        np.roll(np.roll(grid,  1, axis=0),  1, axis=1) +  # Верх-лево
        np.roll(np.roll(grid,  1, axis=0), -1, axis=1) +  # Верх-право
        np.roll(np.roll(grid, -1, axis=0),  1, axis=1) +  # Низ-лево
        np.roll(np.roll(grid, -1, axis=0), -1, axis=1) +  # Низ-право
        np.roll(grid,  1, axis=0) +                      # Верх
        np.roll(grid, -1, axis=0) +                      # Низ
        np.roll(grid,  1, axis=1) +                      # Лево
        np.roll(grid, -1, axis=1)                        # Право
    )

    # Убираем соседей для клеток за пределами игрового поля (принимаем, что поле конечное, если нет - убираем нижние четыре строчки)
    neighbors[0, :] = 0       # Верхняя граница
    neighbors[-1, :] = 0      # Нижняя граница
    neighbors[:, 0] = 0       # Левая граница
    neighbors[:, -1] = 0      # Правая граница
    
    return neighbors


def step(grid):
    # Выполнение одного шага симуляции
    neighbors = count_neighbors(grid)
    
    # Применяем правила игры, обновляя клетки
    new_grid = (neighbors == 3) | ((grid == 1) & (neighbors == 2))

    # Обнуляем клетки на границах (поскольку они не обновляются (принимаем, что поле конечное, если нет - убираем нижние четыре строчки))
    new_grid[0, :] = 0
    new_grid[-1, :] = 0
    new_grid[:, 0] = 0
    new_grid[:, -1] = 0
    
    return new_grid.astype(int)


def print_grid(grid):
    # Выводит сетку в консоль с помощью библиотеки os

    # Очищает консоль
    if os.name == "nt":
        os.system("cls")
    else:
        os.system("clear")
 
    # вывод на консоль матрицу в виде поля, где 'O' - живая клетка, '.' - мёртвая
    for row in grid:
        row_string = ''
        for cell in row:
            if cell:
                row_string += 'O '
            else:
                row_string += '. '
        print(row_string.strip())


def main():
    # Инициализация начального состояния в виде матрицы, заполненной нулями
    grid = np.zeros((20, 20), dtype=int)
    
    # Пример заполнения, можно изменять
    grid[1, 2] = grid[2, 3] = grid[3, 1] = grid[3, 2] = grid[3, 3] = 1
    grid[5, 8] = grid[4, 8] = grid[6, 8] = 1
    grid[18, 19] = grid[18, 18] = grid[18, 17] = grid[17, 18] = grid[19, 18] = 1

    generations = 50  # Количество поколений? можно изменить
    for gen in range(generations):
        print(f"Generation {gen}")
        print_grid(grid)
        grid = step(grid)
        time.sleep(0.15) 


if __name__ == "__main__":
    main()
