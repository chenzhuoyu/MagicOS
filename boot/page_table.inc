; Page table attributes

%define PG_P			0x00000001
%define PG_RW			0x00000002
%define PG_USER			0x00000004
%define PG_PWT			0x00000008
%define PG_PCD			0x00000010
%define PG_ACCESSED		0x00000020
%define PG_DIRTY		0x00000040
%define PG_PS_PAT		0x00000080
%define PG_GLOBAL		0x00000100