# Algorithmik

### Bauen:

- Möglichkeit 1: `make`
- Möglichkeit 2: `cmake -S . -B build` und `cmake --build build`

Das Programm wird kompiliert und führt die Suche der schwachen Zusammenhangskomponenten und den Dijkstra-Algorithmus nacheinander auf den Dateien `graph.fmi` und `queries.txt` aus.

### Ausführen:

Die Pfade für die benötigten Dateien können optional auch explizit gemacht werden:

```
Dijkstra [-graph graph_file] [-queries query_file] [-results results_file] [-mode mode]
```
`-mode` $\in$ {`components`, `distances`, `both`}  bestimmt, welche Aufgabe ausgeführt werden soll.

Die Standardparameter sind `graph_file = graph.fmi`, `query_file = queries.txt`, `results_file = result.txt` und `mode = both`