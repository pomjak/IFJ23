# ifj23

## GIT configuration
```bash
git config --global user.name "name"
git config --global user.email "email@domain.cz"
git config --global pull.rebase true
```

## Clonning of repository

```bash
git clone https://gitlab.com/en1qc/ifj23.git
```

## Adding of new changes
```bash
git checkout work
git pull
git branch <name of new branch>
git checkout <name of new branch>

# Making of changes (commits)

# creating commit
git add .
git commit -m "name of commit"

git push
```

## Testing and building

build without debug information
```bash
make build
```

build with debug information
```bash
make debug #errors
make debug level=1 #errors and warnings
make debug level=2 #errors, warnings and debug
```

run automatic tests
```bash
make --silent test
```

## Debug functions
```c
DEBUG_PRINT() //takes parameters as printf()

WARNING_PRINT() //takes parameters as printf()

ERROR_PRINT() //takes parameters as printf()
```
## Eval
```
Procentuální hodnocení modulů překladače:
Lexikální analýza (detekce chyb): 91 % (147/161)
Syntaktická analýza (detekce chyb): 77 % (168/216)
Sémantická analýza (detekce chyb): 82 % (373/450)
Interpretace přeloženého kódu (základní): 87 % (237/272)
Interpretace přeloženého kódu (výrazy, vest. funkce): 95 % (168/176)
Interpretace přeloženého kódu (komplexní): 25 % (77/308)
OVERLOAD 0 % (0/99)
INTERPOLATION 0 % (0/100)
BOOLTHEN 0 % (0/97)
CYCLES 0 % (0/100)
FUNEXP 0 % (0/151)
Celkem bez rozšíření: 73 % (1170/1583)
```
