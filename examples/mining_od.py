import desbordante
import pandas

algo = desbordante.Order()

df = pandas.read_csv('datasets/cargo_od.csv', sep=',', header=0)

algo.load_data(df)
algo.execute()

od = algo.get_valid_ods()
print("Columns from left hand side order columns from right hand side:")
for lhs, rhs in od:
    lhs_names = list(df.columns[lhs])
    rhs_names = list(df.columns[rhs])
    print(lhs_names, "->", rhs_names)
