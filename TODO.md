# TODO

Punti aperti di CVL, ordinati per priorità (Critica / Alta / Media / Bassa).
Qui non ci sono bug noti: sono decisioni in sospeso e miglioramenti rimandati,
tenuti di proposito fuori dal README.

## Alta — decisioni di progetto aperte

1. **`strength` del detector `subpixel`: 40 o 150.**
   In `include/subpixel.hpp` (`utils::filterKeyPoints`) il codice storico
   impostava `strength = 150` su una struct locale mai usata, quindi il valore
   effettivo è sempre stato il default **40** (mantenuto per compatibilità di
   comportamento, vedi la nota nel codice). Decidere il valore voluto provando
   entrambi su dati di calibrazione reali.

## Media

- **Test mancanti** (dettagli e motivazioni in `tests/README.md`):
  - Trasformare le aree coperte solo da smoke test (`enhance`, `denoising`,
    `subpixel "new"`) in test a oracolo, con output di riferimento verificati a mano.
  - Aggiungere `tests/data/` con un'immagine reale di scacchiera e i corner
    attesi, per coprire da capo a fondo `chessboard::get` e le funzioni stereo
    `pose::*`.
  - Death test basati su `fork` per i percorsi di `log::error()`, compresi i
    controlli aggiunti nella revisione del 2026-07-16: numero di immagini e di
    medie locali nel deflickering, validazione della maschera in `threshold::*`,
    controllo del tipo in `starflag`, limite sul frame in `corners_t`,
    validazione del modo in `contours::get`.

## Bassa

- **Policy `-Wconversion` (decisione presa, da non riaprire senza motivo).**
  Con `-Wconversion` il codice CVL genera ~183 warning di conversioni implicite
  (double→int su coordinate, int→uchar su intensità), esaminati a campione e
  intenzionali. La build di riferimento resta `-Wall -Wextra -Wpedantic`
  (pulita, con gli header OpenCV via `-isystem`). Non silenziare in massa con
  cast cosmetici; eventuali cast espliciti solo su file mirati e con revisione.

- **`utils::medianFirstNNDistance` con un solo elemento restituisce `FLT_MAX`**
  (comportamento documentato: un solo blob → un solo cluster in
  `clustering::NN`). Valutare se un giorno serva un contratto più esplicito.

- **Ottimizzazioni future** (da fare solo se diventano colli di bottiglia reali;
  ogni cambio va accompagnato da un benchmark):
  - **Indice spaziale per il clustering.** `cvl::utils::connectedComponents` e
    `cvl::utils::medianFirstNNDistance` (in `utils.hpp`) sono a forza bruta,
    O(n²) nel numero di blob. Con molti blob una griglia spaziale uniforme (o un
    k-d tree) porterebbe la ricerca dei vicini a circa O(n).
  - **Componenti connesse con OpenCV.** `cvl::labeling::get` usa una union-find
    scritta a mano su una matrice di label contigua. Sostituirne il nucleo con
    `cv::connectedComponentsWithStats` (ottimizzata SIMD, connettività 4/8)
    sarebbe probabilmente più veloce; è un refactor più grosso perché le liste
    di pixel per blob vanno comunque raccolte dopo.
