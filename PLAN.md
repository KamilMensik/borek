# 2D Herni engine na vytvareni her ve stylu ery NES / SNES  

* 2 Casti.  
    * Editor  
    * Runtime  

## Editor  
> V ramci editoru pujdou vytvaret urovne do hry.  
> Zakladni stavebni bloky budou entity, kterym lze pridat Obrazek, nebo jine veci.  
> Engine bude prijmat (snad) vetsinu formatu obrazku.  
> Drag&Drop funkcionalita  
> * Bude existovat lista, ve ktere budou slozky/assety (obrazky, hudba apod). Pretahnutim napr obrazku z listy na entitu se obrazek prida. A ihned se zacne vykreslovat.  
> * Bude existovat lista, ktera reprezentuje aktualni stav sceny. Pujde v ni hybat / rotovat s entitami a tim navrhovat urovne.  
> * Podpora pro tilemapy. Mozna integrovany tilemap editor.  
>
> Take bude existovat lista, ve ktere bude videt vsechny nastavene komponenty prave vybrane entity (po kliknuti na ni). A bude je mozne upravovat / pridavat / odstranovat.  
> Serializace / deserializace jednotlivych urovni.  
> Podpora spritesheetu / animovanych obrazku (pujde nastavit jak rychle animace pojede)  
> Podpora externiho scriptovani (Ruby nebo Lua nejpravdepodobneji).  
> Prehravani hudby / zvukovych efektu / jinych veci.   
> Scena muze mit libovolnou velikost. (Pujde nastavit). V ramci ni se pujde pohybovat pomoci pohybu kamery.  
> Moznost ukladani / nacitani dat. (Ulozeni maximalniho skore, stavu hry apod)  
> Pomoci tohohle by melo jit vytvorit vsechny veci potrebne k 2D hram NES stylu. (napr: pozadi: ctverec s obrazkem, ktery se bude pohybovat / opakovat spolecne s pohybem hrace, hrac: entita co ma nejaky obrazek a pripojeny script, ktery ho pohybuje po zmacknuti klaves WASD)  
> Event system.  
> Zpracovani vstupu.  
> Physics system (mozna).  
> Nativni podpora vypoctu kolizi enginem. (Collider komponenta)  
> Vsechno vykreslovani bude reseno enginem. Ne programatorem. (Ale API bude pristupna, takze pujde kodem vykreslit ctverec napriklad)  
> Zatim plan jen pro vykreslovani obdelniku, ale melo by byt bezproblemove pridat vykreslovani jinych primitiv.  
> Engine sam hlida a vola udalosti (OnEvent, OnUpdate, OnCreate, apod.)  
> Engine pobezi na vlastnim ECS systemu, ale programovat se v nem nejspis bude objektove. Neco ve stylu:  
> ```  
>   # Pseudo ruby kod  
>   class Player < Borek::ScriptableObject  
>     
>       def OnUpdate(delta)  
>           if (Borek::Input::IsKeyPressed(Borek::KeyCode::W))  
>               GetComponent(Transform).y += 20  
>           end  
>       end  
>  
>   end  
> ```  
> Engine by mel byt navrzen tak, aby programator, co neumi s jazyky jako je C++ mohl pouzivat ciste dany scriptovaci jazyk a nebyl jakkoliv omezen funkcionalitou.  
  
## Runtime  
> Bude schopny "precist" hru vytvorenou editorem a spustit ji.  
