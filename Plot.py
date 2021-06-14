from matplotlib import pyplot
def main():
    # encrypt
    pyplot.figure(1)
    tempoAssembly = [0, 211242, 313110, 594224, 714763, 1043108, 1130812, 1213920, 1311339]
    tempoC = [0, 262490, 382199, 589096, 721741, 1049203, 1138523, 1220143, 1319517]
    dimensione_file = [0, 29930, 43294, 68598, 98074, 125790, 140754, 154890, 166710]

    x = [i*10 for i in range(max(dimensione_file)+1)]
    y = [i for i in range(max(dimensione_file)+1)]

    pyplot.plot(tempoAssembly, dimensione_file, label="Andamento Assembly")
    pyplot.plot(tempoC, dimensione_file, label="Andamento C")
    pyplot.xlabel("tempo (ms) esecuzione del file")
    pyplot.ylabel("dimensione del file (Byte)")
    pyplot.locator_params(axis="both", integer=True, tight=True)
    pyplot.legend()
    pyplot.show()

    pyplot.figure(2)
    tempoAssembly = [18, 87, 160, 229, 305, 373, 552, 622, 701, 776, 864]
    tempoC = [23, 108, 197, 285, 376, 464, 687, 774, 868, 954, 1051]
    dimensione_file = [1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 101]

    x = [i*10 for i in range(max(dimensione_file)+1)]
    y = [i for i in range(max(dimensione_file)+1)]

    pyplot.plot(tempoAssembly, dimensione_file, label="Andamento Assembly")
    pyplot.plot(tempoC, dimensione_file, label="Andamento C")
    pyplot.xlabel("tempo (ms) esecuzione del file")
    pyplot.ylabel("dimensione del file (Byte)")
    pyplot.locator_params(axis="both", integer=True, tight=True)
    pyplot.legend()
    pyplot.show()

if __name__ == "__main__":
    main()
