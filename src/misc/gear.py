#####################################################33
# Base Status:
#  - pstats: Primary attributes
#  - vitaly: Vitality
#  - lifepc: Life %
#  - crithc: Critical Hit Chance
#  - crithd: Critial Hit Damage
#  - atcksp: Increased Attack Speed
#  - killxp: Kill Experience
#  - goldfd: Gold Find
#  - eledmg: Elemental Damage
#  - skldmg: Skill Damage
#  - allres: All Resistance
#  - armorr: Armor
#  - basdmg: Base Damage
#
# Metrics:
#  - DMG: Damage
#  - SDM: Skill Damage
#  - EDM: Elemental Damage
#  - MDM: Skill+Elemental Damage
#  - ATS: Hits per Seconds
#  - TGS: Toughness
#  - HTP: HitPoints
#
# Properties:
#  - prop_pat: Primary Attributes
#  - prop_vit: Vitality
#  - prop_lpc: Life %
#  - prop_chc: Critical Hit Chance
#  - prop_chd: Critial Hit Damage
#  - prop_ias: Increased Attack Speed
#  - prop_kxp: Kill Experience
#  - prop_gfd: Gold Find
#  - prop_eld: Elemental Damage
#  - prop_skd: Skill Damage
#  - prop_ars: All Resistance
#  - prop_arm: Armor
#  - prop_bdm: Base Damage
#
# Itens:
# - Helm
# - Pauldrons
# - Amulet
# - Chest
# - Bracer
# - Glove
# - Belt
# - Ring1
# - Ring2
# - Pants
# - Weapon
# - Offhand
# - Boots
#####################################################
from enum import Enum
Animal = Enum('Animal', 'ant bee cat dog')

#####################################################
# Properties types
# (<acronym>, <name of property>, <status attribute modified>)
#####################################################
propTypes = [
	("PA", "Primary attributes", 'pattri'),
	("VT", "Vitality", 'vitaly'),
	("LF", "Life %", 'lifepc'),
	("CC", "Critical Hit Chance", 'crithc'),
	("CD", "Critial Hit Damage", 'crithd'),
	("AS", "Increased Attack Speed", 'atcksp'),
	("KE", "Kill Experience", 'killxp'),
	("GF", "Gold Find", 'goldfd'),
	("ED", "Elemental Damage", 'eledmg'),
	("SD", "Skill Damage", 'skldmg'),
	("AR", "All Resistance", 'allres'),
	("AM", "Armor", 'armorr'),
	("BD", "Base Damage", 'basdmg')
]

#####################################################
# Items types
# (<acronym>, <name of item>, <standart list of properties>)
#####################################################
itensTypes = [
	("HE","Helm", [
		(),
		(),
		()]),
	("PA","Pauldrons", [
		(),
		(),
		()]),
	("AM","Amulet", [
		(),
		(),
		()]),
	("CH","Chest", [
		(),
		(),
		()]),
	("BR","Bracer", [
		(),
		(),
		()]),
	("GL","Glove", [
		(),
		(),
		()]),
	("BE","Belt", [
		(),
		(),
		()]),
	("R1","Ring1", [
		(),
		(),
		()]),
	("R2","Ring2", [
		(),
		(),
		()]),
	("PA","Pants", [
		(),
		(),
		()]),
	("WE","Weapon", [
		(),
		(),
		()]),
	("OF","Offhand", [
		(),
		(),
		()]),
	("BO","Boots", [
		(),
		(),
		()])
]

#####################################################
# The Status instance
#####################################################
class Status:
	def __init__(self):
		# base status
		self.pattri = 0    # Primary attributes
		self.vitaly = 0    # Vitality
		self.lifepc = 1.0  # Life %
		self.crithc = 0.05 # Critical Hit Chance
		self.crithd = 0.5  # Critial Hit Damage
		self.atcksp = 1.0  # Increased Attack Speed
		self.killxp = 0    # Kill Experience
		self.goldfd = 0    # Gold Find
		self.eledmg = 1.0  # Elemental Damage
		self.skldmg = 1.0  # Skill Damage
		self.allres =      # All Resistance
		self.armorr = 0    # Armor
		self.basdmg = 0    # Base Damage
		# metrics
		self.dmg = 0       # Damage
		self.sdm = 1.0     # Skill Damage
		self.edm = 1.0     # Elemental Damage
		self.mdm = 1.0     # Skill+Elemental Damage
		self.ats = 1.0     # Hits per Seconds
		self.tgs = 0       # Toughness
		self.htp = 0       # HitPoints

#####################################################
# An item instance.
#  Ex.: Helm
#####################################################
class Item:
	def __init__(self, name, maxprop, props):
		self.name = name          # its name
		self.maxprop = maxprops   # max number of properties
		self.props = props        # list of available properties

#####################################################
# An item property
#  Ex.: +Vitality
#####################################################
class Property:
	# Constructor
	def __init__(self, name, attrname, lo, hi, item):
		self.name = name               # name of property
		self.attrname = attrname       # name of the status attribute it modifies
		self.lo = lo                   # lower possible value for the property
		self.hi = hi                   # higher possible value for the property
		self.quality = 0               # quality of property
		self.value = lo                # the value of the property
		self.item = item               # the item owner of the property
		self.status = item.status      # the status instance
	
	# Set the quality of property (how good it is)
	def setQuality(self, quality=0.5):
		self.quality = quality
		self.value = self.lo + (self.hi - self.hi)/self.quality
	
	# Apply the property on status
	def applyProp(self):
		setattr(self.status, getattr(self.status, self.attrname) + self.value)

	# Remove the property from status
	def removeProp(self):
		setattr(self.status, getattr(self.status, self.attrname) - self.value)

