#const clash_weight = 2.
#const same_room_weight = 1.
#const gap_weight = 2.

% Assign lectures to classrooms
L { assigned(C, D, T, S) : day(D), timeslot(T) } L :- course(C, _, M, S, L).
1 { assigned(C,D,T,R,S) : classroom(R,_) } 1 :- assigned(C,D,T,S).

% Constraint: Two lectures cannot take place in the same room in the same timeslot
:- not { assigned(C,D,T,R,S) : course(C, _, _, _,_) } 1, classroom(R,_), day(D), timeslot(T).  

% Constraint: Classroom capacity cannot be exceeded
:- assigned(C, D, T, R, S), course(C, _, M, S, _), classroom(R, N), M > N.

% Constraint: Non-service undergraduate courses cannot occupy a classroom longer than 2 hours consecutively
:-  assigned(C, D, T1, R, S), assigned(C, D, T2, R, S),  T2 = T1 + 2.

% Constraint: If a non-service undergraduate course has split into three sections, at least one hour must be conducted at BMB1
has_three_sections(C) :- course(C, _, _, "3s", _).

at_least_one_hour_in_bmb_s2(C, "2s") :-  assigned(C, _, _, "BMB-1", "2s").
at_least_one_hour_in_bmb_s1(C, "1s") :-  assigned(C, _, _, "BMB-1", "1s").
at_least_one_hour_in_bmb_s3(C, "3s") :-  assigned(C, _, _, "BMB-1", "3s").

:- has_three_sections(C), not at_least_one_hour_in_bmb_s2(C, "2s").
:- has_three_sections(C), not at_least_one_hour_in_bmb_s3(C, "3s").
:- has_three_sections(C), not at_least_one_hour_in_bmb_s1(C, "1s").


% Constraint: If a non-service undergraduate level course has splitted into two sections, all hours must be conducted at BMB1 at least
has_two_sections(C) :- course(C, _, _, "1s", _), course(C, _, _, "2s", _), not course(C, _, _, "3s", _).

all_hours_in_bmb1_1s(C, "1s") :-   course(C, _, _, "1s", N),  N{ assigned(C, _, _, "BMB-1", "1s") }.
all_hours_in_bmb1_2s(C, "2s") :-   course(C, _, _, "2s", N),  N{ assigned(C, _, _, "BMB-1", "2s") }.

:- has_two_sections(C), not all_hours_in_bmb1_1s(C, "1s").
:- has_two_sections(C), not all_hours_in_bmb1_2s(C, "2s").


% Constraint: Same sections of the two same-year must courses cannot clash
S1 != S2 :- assigned(C1,D,P,_,S1), assigned(C2,D,P,_,S2), course(C1,Y,_,_,_), course(C2,Y,_,_,_), C1!=C2, not elective(C1, _), not elective(C2, _).

% Constraint: Sections of the same course should not overlap
:- assigned(C, D, T, _, S1), assigned(C, D, T, _, S2), S1 != S2.

% Assign instructors to courses
1 { assigned(C, I, D, T, R, S) : instructor(I, C) } 2 :- assigned(C, D, T, R, S).

% Since CENG331 is a special case regarding clasrooms (it has 1 section with 200 people, so they dont fit anywhere in our department)
% We assigned it individually to G111 just like it has been done this semester
3 { assigned(5710331, "EROL_SAHIN", D, T, "G111", "1s") : day(D), timeslot(T)}3.

%Constraint: No two instructors teach the same section for the same course
:- assigned(C, I1, _, _, _, S), assigned(C, I2, _, _, _, S), instructor(I1, C), instructor(I2, C), I1 != I2.

% Constraint: If there are two teachers for a course, they both teach at least one section
:- instructor(I,C), not assigned(C,I,_,_,_,_).

% Constraint: If the teacher of the course is unavailable to teach that course at a given time slot,
% then no lecture of the course can be scheduled at that time slot.
:- assigned(C, I, D, T, _, _), busy(I, D, T), instructor(I, C).

% If there are 1-hour lectures in more than 1 days, the best option would be to group them together, so we don't accept them
:- assigned(C, I, D1, _, _, S), assigned(C, I, D2, _, _, S), D1!=D2, 1{assigned(C,_,D1,_,_,S)}1, 1{assigned(C,_,D2,_,_,S)}1.



% Constraint: If there are two lectures of a course having the same section in the same day, they should be back-to-back
% and be in the same classroom
T1-T2 =1 :- assigned(C, _, D, T1,_, S), assigned(C, _, D, T2,_, S), T1 > T2.
R1=R2 :- assigned(C, _, D, T1, R1, S), assigned(C, _, D, T2, R2, S), T1 > T2.

% Soft Constraint: Minimize the gap between courses of the same year on the same day
 penalty("time_gap", time_gap(Y, D, Gap), Gap * gap_weight) :- Y!="5y", Y!="0y", course(C1, Y, _, S1, _), course(C2, Y, _, S2, _), C1 != C2, 
    assigned(C1, D, T1, _, S1), assigned(C2, D, T2, _, S2), not elective(C1,Y), not elective(C2,Y), Gap = |T2 - T1|.

% Soft Constraint: Minimize the elective clashes
penalty("elective_clash", clash(C1, C2, D, P), clash_weight) :- assigned(C1, D, P, _), assigned(C2, D, P, _), elective(C1, "4y"), elective(C2, "4y"), C1 != C2.

% Soft Constraint: All the sections of a course should be conducted in the same clasroom if possible
penalty("same_room", same_room(R1, R2, C, S), same_room_weight) :-  assigned(C, _, _, _, R1, S), assigned(C, _, _, _, R2, S), R1 != R2.


#minimize { P,C,S : penalty(S,C,P) }.


#show assigned/6.
