#ifndef ORGANISM
#define ORGANISM

#include <iostream>
#include <optional>
#include <stdbool.h>

template <typename species_t, bool can_eat_meat, bool can_eat_plants> 
class Organism {
private:
    const species_t species;
    uint64_t vitality;
public:
    constexpr Organism(species_t const &species, uint64_t vitality) : 
             species(species), vitality(vitality){};

    uint64_t get_vitality() const {
        return vitality;
    }

    const species_t &get_species() {
        return species;
    }

    void add_vitality(uint64_t vit) const {
        this->vitality += vit;
    }

    void die() const {
        this->vitality = 0;
    }

    bool is_dead() const {
        return (vitality == 0);
    }
};

template<typename species_t>
using Carnivore = Organism<species_t, true, false>;

template<typename species_t>
using Omnivore = Organism<species_t, true, true>;

template<typename species_t>
using Herbivore = Organism<species_t, false, true>;

template<typename species_t>
using Plant = Organism<species_t, false, false>;

//zamiana dwoch elementow w tupli
template <typename species_t,  bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>> swap_result(
        std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
                Organism<species_t, sp2_eats_m, sp2_eats_p>>) {
    return;
}

//sprawdza czy ten sam gatunek
template <typename species_t,  bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr bool sameSpecies(Organism<species_t, sp1_eats_m, sp1_eats_p> &organism1,
                 Organism<species_t, sp2_eats_m, sp2_eats_p> &organism2) {
    return (sp1_eats_m == sp2_eats_m && sp1_eats_p == sp2_eats_p &&
            organism1.get_species() == organism2.get_species());
}

//gody
template <typename species_t,  bool sp_eats_m, bool sp_eats_p>
constexpr Organism<species_t, sp_eats_m, sp_eats_p> mating(
                        Organism<species_t, sp_eats_m, sp_eats_p> &organism1,
                        Organism<species_t, sp_eats_m, sp_eats_p> &organism2) {
    return Organism<species_t, sp_eats_m, sp_eats_p> (organism1.get_species(),
              (organism1.get_vitality() + organism2.get_vitality()) / 2);
}


//pierwszy je drugiego (możliwe)
template <typename species_t, bool sp1_eats_p>
constexpr std::tuple<Organism<species_t, true, sp1_eats_p>, Herbivore<species_t>>
encounter_worker(Organism<species_t, true, sp1_eats_p> &organism1,
          Herbivore<species_t> &organism2) {
    if(organism1.get_vitality() > organism2.get_vitality()){
        organism1.add_vitality(organism2.get_vitality()/2);
        organism2.die();
    }
    return std::make_tuple(organism1, organism2);
}

//drugi je pierwszego (może)
template <typename species_t, bool sp2_eats_p>
constexpr std::tuple<Herbivore<species_t>, Organism<species_t, true, sp2_eats_p>>
encounter_worker(Herbivore<species_t> &organism1,
          Organism<species_t, true, sp2_eats_p> &organism2) {
    return swap_result(encounter_worker(organism2, organism1));
}

//obaj mogą się zjeść
template <typename species_t, bool sp1_eats_p, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, true, sp1_eats_p>,
                     Organism<species_t, true, sp2_eats_p>>
encounter_worker(Organism<species_t, true, sp1_eats_p> &organism1,
          Organism<species_t, true, sp2_eats_p> &organism2) {
    if (sameSpecies(organism1, organism2) && (!organism1.is_dead() && !organism2.is_dead())) {
        return std::make_tuple(organism1, organism2, mating(organism1, organism2));
    }
    if(organism1.get_vitality() > organism2.get_vitality()){
        organism1.add_vitality(organism2.get_vitality()/2);
        organism2.die();
    } else{
        organism2.add_vitality(organism1.get_vitality()/2);
        organism1.die();
    }
    return std::make_tuple(organism1, organism2);
}

//ktoś je roślinę
template <typename species_t, bool sp1_eats_m>
constexpr std::tuple<Organism<species_t, sp1_eats_m, true>,
                     Plant<species_t>>
encounter_worker(Organism<species_t, sp1_eats_m, true> &organism1,
          Plant<species_t> &organism2) {
    if(organism1.get_vitality() > organism2.get_vitality()){
        organism1.add_vitality(organism2.get_vitality());
        organism2.die();
    }
    return std::make_tuple(organism1, organism2);
}

//ktoś je roślinę ale w drugą stronę
template <typename species_t, bool sp2_eats_m>
constexpr std::tuple<Plant<species_t>,
                     Organism<species_t, sp2_eats_m, true>>
encounter_worker(Plant<species_t> &organism1,
          Organism<species_t, sp2_eats_m, true> &organism2) {
    return swap_result(encounter_worker(organism2, organism1));
}

template <typename species_t>
constexpr std::tuple<Herbivore<species_t>, Herbivore<species_t>>
encounter_worker(Herbivore<species_t> &organism1, Herbivore<species_t> &organism2) {
    if (sameSpecies(organism1, organism2) && (!organism1.is_dead() && !organism2.is_dead())) {
        return std::make_tuple(organism1, organism2, mating(organism1, organism2));
    }
    return std::make_tuple(organism1, organism2);
}

template <typename species_t,  bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
requires (sp1_eats_m || sp1_eats_p || sp2_eats_m || sp2_eats_p)
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>,
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2){
    return encounter_worker(organism1, organism2);
}


template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args){
    return (std::get<0>(encounter_worker(organism1, args)), ...);
}

#endif 