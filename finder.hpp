#pragma once
#include "index_creator.hpp"
template < typename C = char, typename Index_Creator = SimpleInvertIndexCreatorWithPos< std::basic_string< C > > >
struct Finder;
#include "finder.tpp"
