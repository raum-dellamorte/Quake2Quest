/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * Animations for final boss stage 1.
 *
 * =======================================================================
 */

#define FRAME_attak101 0
#define FRAME_attak102 1
#define FRAME_attak103 2
#define FRAME_attak104 3
#define FRAME_attak105 4
#define FRAME_attak106 5
#define FRAME_attak107 6
#define FRAME_attak108 7
#define FRAME_attak109 8
#define FRAME_attak110 9
#define FRAME_attak111 10
#define FRAME_attak112 11
#define FRAME_attak113 12
#define FRAME_attak114 13
#define FRAME_attak115 14
#define FRAME_attak116 15
#define FRAME_attak117 16
#define FRAME_attak118 17
#define FRAME_attak201 18
#define FRAME_attak202 19
#define FRAME_attak203 20
#define FRAME_attak204 21
#define FRAME_attak205 22
#define FRAME_attak206 23
#define FRAME_attak207 24
#define FRAME_attak208 25
#define FRAME_attak209 26
#define FRAME_attak210 27
#define FRAME_attak211 28
#define FRAME_attak212 29
#define FRAME_attak213 30
#define FRAME_death01 31
#define FRAME_death02 32
#define FRAME_death03 33
#define FRAME_death04 34
#define FRAME_death05 35
#define FRAME_death06 36
#define FRAME_death07 37
#define FRAME_death08 38
#define FRAME_death09 39
#define FRAME_death10 40
#define FRAME_death11 41
#define FRAME_death12 42
#define FRAME_death13 43
#define FRAME_death14 44
#define FRAME_death15 45
#define FRAME_death16 46
#define FRAME_death17 47
#define FRAME_death18 48
#define FRAME_death19 49
#define FRAME_death20 50
#define FRAME_death21 51
#define FRAME_death22 52
#define FRAME_death23 53
#define FRAME_death24 54
#define FRAME_death25 55
#define FRAME_death26 56
#define FRAME_death27 57
#define FRAME_death28 58
#define FRAME_death29 59
#define FRAME_death30 60
#define FRAME_death31 61
#define FRAME_death32 62
#define FRAME_death33 63
#define FRAME_death34 64
#define FRAME_death35 65
#define FRAME_death36 66
#define FRAME_death37 67
#define FRAME_death38 68
#define FRAME_death39 69
#define FRAME_death40 70
#define FRAME_death41 71
#define FRAME_death42 72
#define FRAME_death43 73
#define FRAME_death44 74
#define FRAME_death45 75
#define FRAME_death46 76
#define FRAME_death47 77
#define FRAME_death48 78
#define FRAME_death49 79
#define FRAME_death50 80
#define FRAME_pain101 81
#define FRAME_pain102 82
#define FRAME_pain103 83
#define FRAME_pain201 84
#define FRAME_pain202 85
#define FRAME_pain203 86
#define FRAME_pain301 87
#define FRAME_pain302 88
#define FRAME_pain303 89
#define FRAME_pain304 90
#define FRAME_pain305 91
#define FRAME_pain306 92
#define FRAME_pain307 93
#define FRAME_pain308 94
#define FRAME_pain309 95
#define FRAME_pain310 96
#define FRAME_pain311 97
#define FRAME_pain312 98
#define FRAME_pain313 99
#define FRAME_pain314 100
#define FRAME_pain315 101
#define FRAME_pain316 102
#define FRAME_pain317 103
#define FRAME_pain318 104
#define FRAME_pain319 105
#define FRAME_pain320 106
#define FRAME_pain321 107
#define FRAME_pain322 108
#define FRAME_pain323 109
#define FRAME_pain324 110
#define FRAME_pain325 111
#define FRAME_stand01 112
#define FRAME_stand02 113
#define FRAME_stand03 114
#define FRAME_stand04 115
#define FRAME_stand05 116
#define FRAME_stand06 117
#define FRAME_stand07 118
#define FRAME_stand08 119
#define FRAME_stand09 120
#define FRAME_stand10 121
#define FRAME_stand11 122
#define FRAME_stand12 123
#define FRAME_stand13 124
#define FRAME_stand14 125
#define FRAME_stand15 126
#define FRAME_stand16 127
#define FRAME_stand17 128
#define FRAME_stand18 129
#define FRAME_stand19 130
#define FRAME_stand20 131
#define FRAME_stand21 132
#define FRAME_stand22 133
#define FRAME_stand23 134
#define FRAME_stand24 135
#define FRAME_stand25 136
#define FRAME_stand26 137
#define FRAME_stand27 138
#define FRAME_stand28 139
#define FRAME_stand29 140
#define FRAME_stand30 141
#define FRAME_stand31 142
#define FRAME_stand32 143
#define FRAME_stand33 144
#define FRAME_stand34 145
#define FRAME_stand35 146
#define FRAME_stand36 147
#define FRAME_stand37 148
#define FRAME_stand38 149
#define FRAME_stand39 150
#define FRAME_stand40 151
#define FRAME_stand41 152
#define FRAME_stand42 153
#define FRAME_stand43 154
#define FRAME_stand44 155
#define FRAME_stand45 156
#define FRAME_stand46 157
#define FRAME_stand47 158
#define FRAME_stand48 159
#define FRAME_stand49 160
#define FRAME_stand50 161
#define FRAME_stand51 162
#define FRAME_walk01 163
#define FRAME_walk02 164
#define FRAME_walk03 165
#define FRAME_walk04 166
#define FRAME_walk05 167
#define FRAME_walk06 168
#define FRAME_walk07 169
#define FRAME_walk08 170
#define FRAME_walk09 171
#define FRAME_walk10 172
#define FRAME_walk11 173
#define FRAME_walk12 174
#define FRAME_walk13 175
#define FRAME_walk14 176
#define FRAME_walk15 177
#define FRAME_walk16 178
#define FRAME_walk17 179
#define FRAME_walk18 180
#define FRAME_walk19 181
#define FRAME_walk20 182
#define FRAME_walk21 183
#define FRAME_walk22 184
#define FRAME_walk23 185
#define FRAME_walk24 186
#define FRAME_walk25 187

#define MODEL_SCALE 1.000000
