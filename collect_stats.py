import os
import re
import csv
import argparse

def parse_filename(filename):
    """
    Разбирает имя файла вида var714_parallel_20_O0.out
    Возвращает: name, threads, optimization
    """
    # Убираем расширение
    base_name = filename.replace('.out', '')
    
    # Разбиваем по подчеркиванию
    parts = base_name.split('_')
    
    # Ожидаем, что минимум 3 части: Name..._Threads_Opt
    if len(parts) < 3:
        # Если формат не подходит, возвращаем None
        return None, None, None

    # Последняя часть - Оптимизация (O0, O1 и т.д.)
    optimization = parts[-1]
    
    # Предпоследняя часть - Потоки
    threads = parts[-2]
    
    # Всё, что было до этого - Имя (собираем обратно)
    name = "_".join(parts[:-2])
    
    return name, threads, optimization

def get_time_from_file(filepath):
    """
    Ищет строку вида "Time: 0.006598" ИЛИ "Time taken: 0.020000"
    """
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            
            # Регулярка объяснение:
            # Time       - ищем слово Time
            # (?: taken)? - необязательная группа: пробел и слово taken
            # :\s+       - двоеточие и пробелы
            # ([\d\.]+)  - захватываем число (цифры и точку)
            match = re.search(r"Time(?: taken)?:\s+([\d\.]+)", content)
            
            if match:
                return match.group(1)
    except Exception as e:
        print(f"Ошибка чтения {filepath}: {e}")
    return None

def main():
    parser = argparse.ArgumentParser(description='Сбор статистики из логов .out')
    parser.add_argument('--n', required=True, type=str, help='Значение параметра N')
    parser.add_argument('--dir', default='.', help='Директория с логами')
    parser.add_argument('--out', default='results.csv', help='Имя выходного файла')
    
    args = parser.parse_args()
    
    results = []
    
    # Проверка существования директории
    if not os.path.exists(args.dir):
        print(f"Директория {args.dir} не найдена.")
        return

    files = [f for f in os.listdir(args.dir) if f.endswith('.out')]
    print(f"Найдено файлов .out: {len(files)}")
    
    for filename in files:
        # 1. Парсим имя файла
        name, threads, opt = parse_filename(filename)
        
        if not name:
            print(f"Пропущен файл (неверный формат имени): {filename}")
            continue

        # 2. Парсим время внутри файла
        filepath = os.path.join(args.dir, filename)
        time_val = get_time_from_file(filepath)
        
        if time_val is None:
            # Можно заменить на пустую строку или 0, если нужно
            time_val = "N/A" 

        results.append({
            "Name": name,
            "Threads": threads,
            "Optimization": opt,
            "Time": time_val,
            "N": args.n
        })

    # Сортировка: сначала по Имени, потом по числу потоков (как число)
    results.sort(key=lambda x: (x['Name'], int(x['Threads']) if x['Threads'].isdigit() else 0))

    if results:
        fieldnames = ["Name", "Threads", "Optimization", "Time", "N"]
        with open(args.out, 'w', newline='', encoding='utf-8') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            writer.writerows(results)
        print(f"Готово! Обработано {len(results)} записей. Результат в '{args.out}'")
    else:
        print("Данные не найдены.")

if __name__ == "__main__":
    main()
