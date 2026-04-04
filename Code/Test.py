

import gzip

file_path = r"Y:\1_Semester_I_2026\Algorithms\SimulatingAnnealingProyect\Presentation I\DataSets\TSP\world.tsp.gz"

with gzip.open(file_path, "rt", encoding="utf-8") as f:
    content = f.read()

print(content[:5000])